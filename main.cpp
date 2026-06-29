// TRABALHO DE PROGRAMAÇÃO ORIENTADA A OBJETOS
// Quando alguém quiser usar o líquido para um processo, e o tanque estiver no limite baixo, mesmo que a bomba de reabastecimento esteja aberta, precisa ter um alarme que dispara avisando ao usuário que ele não pode usar esse tanque até que ele atinja o nível baixo de segurança + nível que ele quer usar no processo
// Ver as uilizações da biblioteca stdexcept (erros)
// incluir sensor de temperatura
// Separar os componentes em vários arquivos e pensar nas estratéias (situações possíveis que o programa vai decidir sozinho qual usar)

#include "Bomba.hpp"
#include "CalculadoraDemanda.hpp"
#include "ComandoAlterarSetPoint.hpp"
#include "ComandoDesligarTudo.hpp"
#include "ComandoLigarBombaSaida.hpp"
#include "ComandoResetarAlarmes.hpp"
#include "GeradorJSON.hpp"
#include "GerenciadorComandos.hpp"
#include "GerenciadorManutencao.hpp"
#include "MonitorEquipamento.hpp"
#include "NivelAcesso.hpp"
#include "ParBombas.hpp"
#include "PlantaMistura.hpp"
#include "RegistroSistema.hpp"
#include "RegraControle.hpp"
#include "RegraControleTemperatura.hpp"
#include "RegraFalhaReservatorio.hpp"
#include "RegraNivelAlto.hpp"
#include "RegraNivelBaixo.hpp"
#include "RegraPressaoAlta.hpp"
#include "ReservatorioFrio.hpp"
#include "ReservatorioQuente.hpp"
#include "Sensor.hpp"
#include "SensorNivel.hpp"
#include "SensorPressao.hpp"
#include "SensorTemperatura.hpp"
#include "SensorVazao.hpp"
#include "Serpentina.hpp"
#include "SistemaAcesso.hpp"
#include "StatusManutencao.hpp"
#include "TanqueMistura.hpp"
#include "TipoSensorTemp.hpp"
#include "TipoSerpentina.hpp"
#include "Usuario.hpp"
#include "Valvula.hpp"
#include "Comando.hpp"
#include "NivelAcesso.hpp"
#include "AgendamentoManutencao.hpp"


#include <iostream>
#include <string>
#include <random> // Biblioteca para gerar números aleatórios (o Random Walk do código)
#include <thread> // Biblioteca que deixa pausar a execução do programa
#include <chrono> // Biblioteca para utilizar unidade de tempo
#include <vector> // para listas de usuários e agendamentos de manutenções
#include <stdexcept> // para lançar exceções de acesso negado
#include <fstream> // salvar histórico em arquivo
#include <ctime> //gerar o timestamp real de cada leitura (o timestamp amarra o dado fisico ao tempo real, pra que o operador saiba exatamente quando aconteceu o evento)

using namespace std;
//ID_DUPLA = 61

int main() {

    RegistroSistema registro;
    SistemaAcesso acesso(registro);
    GerenciadorManutencao gerManutencao(registro);
    GeradorJSON json;
    GerenciadorComandos receptor("comandos.json");
    
    string nome, senha;
    cout << "=== LOGIN ===" << endl;
    cout << "Usuario: "; cin >> nome;
    cout << "Senha:   "; cin >> senha;

    if (!acesso.login(nome, senha)) {
        cout << "Login falhou. Encerrando." << endl;
        return 1;
    }

    double tempDesejada, volumeDesejado;
    cout << "\n=== CONFIGURACAO DO PROCESSO ===" << endl;
    cout << "Temperatura desejada (20 a 70 C): ";  cin >> tempDesejada;
    cout << "Volume por minuto desejado (L/min): "; cin >> volumeDesejado;

    registro.registrarAcaoOperador(
        acesso.getNomeAtivo(),
        "Setpoint definido: " + to_string(tempDesejada) +
        "C | Volume: "        + to_string(volumeDesejado) + " L/min");

    // Cria a planta e aplica os parâmetros calculados
    PlantaMistura planta;
    CalculadoraDemanda::Parametros params =
        CalculadoraDemanda::calcular(tempDesejada, volumeDesejado);
    planta.aplicarParametros(params);
    planta.setSetpointTemperatura(tempDesejada);

    cout << "\n[SISTEMA] Parametros calculados automaticamente:" << endl;
    cout << "  Valvula quente:  " << params.AberturaValvulaQuente << "%" << endl;
    cout << "  Valvula fria:    " << params.AberturaValvulaFria   << "%" << endl;
    cout << "  Potencia bombas: " << params.PotenciaBombaQuente   << "%" << endl;

    // Registra todos os monitores no gerenciador de manutenção
    gerManutencao.registrarMonitor(&planta.getBombasQuente().getPrincipal().Monitor);
    gerManutencao.registrarMonitor(&planta.getBombasQuente().getReserva().Monitor);
    gerManutencao.registrarMonitor(&planta.getBombasFria().getPrincipal().Monitor);
    gerManutencao.registrarMonitor(&planta.getBombasFria().getReserva().Monitor);
    gerManutencao.registrarMonitor(&planta.getSerpentinaQ().Monitor);
    gerManutencao.registrarMonitor(&planta.getSerpentinaF().Monitor);

    // Regras de controle em ordem de prioridade
    RegraFalhaReservatorio   regraFalha;
    RegraNivelAlto           regraNivelAlto;
    RegraNivelBaixo          regraNivelBaixo;
    RegraPressaoAlta         regraPressaoAlta;
    RegraControleTemperatura regraTemperatura;

    // Vetor de regras: o loop aplica todas em ordem a cada ciclo
    vector<RegraControle*> regras = {
        &regraFalha,
        &regraNivelAlto,
        &regraNivelBaixo,
        &regraPressaoAlta,
        &regraTemperatura
    };

    const double HORAS_POR_CICLO = 0.5;

    cout << "\n___ SIMULACAO INICIADA | Usuario: "
         << acesso.getNomeAtivo() << " ___" << endl;
    cout << "Comandos:" << endl;
    cout << "  [ENTER] = avancar ciclo" << endl;
    cout << "  s       = alterar setpoint" << endl;
    cout << "  m       = propor manutencao (tecnico)" << endl;
    cout << "  a       = aprovar manutencao (admin)" << endl;
    cout << "  r       = recusar manutencao (admin)" << endl;
    cout << "  h       = ver historico (admin)" << endl;
    cout << "  f       = simular falha serpentina (demonstracao)" << endl;
    cout << "  q       = encerrar" << endl;

    bool modo_auto = false;    //para alternar entre o modo manual (terminal) e o modo automático (site)
    int ciclo = 1;
    while (true) {
        registro.setCiclo(ciclo);

        cout << "\n>>>> CICLO: " << ciclo
             << " | Usuario: " << acesso.getNomeAtivo() << endl;

        // Atualiza toda a planta
        planta.atualizar(HORAS_POR_CICLO);

        // Verifica alertas de manutenção
        gerManutencao.verificarAlertas();

        // Aplica todas as regras em ordem
        planta.resetarAlarmes();
        for (auto* regra : regras) {
            regra->aplicar(planta);
        }

        // Registra alarmes ativos no arquivo
        for (const string& alarme : planta.getAlarmes()) {
            registro.registrarAlarme(alarme);
        }

        //escreve todas as leituras no arquivo json a cada ciclo
        json.escreverLeitura("TQ-MIX-TT", "temperatura_mistura",
            planta.getTemperaturaMistura(), "C",
            planta.getAlarmes().empty() ? "OK" : "ALARME", ciclo);

        json.escreverLeitura("TQ-MIX-LT", "nivel_tanque",
            planta.getNivelTanque(), "%",
            planta.getAlarmes().empty() ? "OK" : "ALARME", ciclo);

        json.escreverLeitura("TQ-MIX-PT", "pressao_saida",
            planta.getPressaoSaida(), "BAR",
            planta.getPressaoSaida() >= 6.5 ? "ALARME" : "OK", ciclo);

        json.escreverLeitura("RES-Q-TT", "temperatura_reservatorio_quente",
            planta.getTempReservatorioQ(), "C",
            planta.getTempReservatorioQ() < 55.0 ? "FALHA" : "OK", ciclo);

        json.escreverLeitura("RES-F-TT", "temperatura_reservatorio_frio",
            planta.getTempReservatorioF(), "C",
            planta.getTempReservatorioF() > 30.0 ? "FALHA" : "OK", ciclo);

        json.escreverAtuador("BOMBA-Q", 
            planta.getBombasQuente().estaUsandoReserva() ? "RESERVA" : "PRINCIPAL",
            planta.getBombasQuente().calcularVazao(50.0), ciclo);

        json.escreverAtuador("BOMBA-F",
            planta.getBombasFria().estaUsandoReserva() ? "RESERVA" : "PRINCIPAL",
            planta.getBombasFria().calcularVazao(50.0), ciclo);

        // escreve alarmes ativos no JSON
        for (const string& alarme : planta.getAlarmes()) {
            json.escreverAlarme(alarme, "media", ciclo);
        }

        //Dispara a função que lê o arquivo. Se o admin apertou algo no site, o receptor vai atualizar os valores dentro dele.
        receptor.atualizarComandos();
        //extrai as ordens vindas do arquivo JSON enviado pelo servidor web
        bool emergencia = receptor.getParadaEmergencia(); 
        double setpoint = receptor.getSetpointTemp();     

        //Aplica as ordens do site (temperatura e overrides das bombas) diretamente na planta
        planta.receberComandosSupervisorio(setpoint, emergencia, receptor.getOverrideBombaQ(), receptor.getOverrideBombaF());

        if (emergencia) {
            std::cout << "ALARME DE EMERGENCIA ATIVADO! Desligando processo." << std::endl;
        } else {
            std::cout << "Controlando temperatura buscando: " << setpoint << " C" << std::endl;
        }

        // Relatório do ciclo
        cout << " [RESERVATORIOS]"
             << " | Quente: " << planta.getTempReservatorioQ() << "C"
             << " | Frio: "   << planta.getTempReservatorioF() << "C"
             << endl;

        cout << " [TANQUE MISTURA]"
             << " | Temp: "   << planta.getTemperaturaMistura() << "C"
             << " | Setpoint: "<< planta.getSetpointTemperatura() << "C"
             << " | Nivel: "  << planta.getNivelTanque()        << "%"
             << " | Pressao: "<< planta.getPressaoSaida()       << " BAR"
             << endl;

        cout << " [MONITOR SERP-Q]"
             << " Temp: "     << planta.getSerpentinaQ().Monitor.getTemperaturaOperacao()
             << "C | Status: "<< planta.getSerpentinaQ().Monitor.getStatusTexto()
             << endl;

        //Se o modo automático estiver ligado, pula a leitura do teclado (cin/getline), 
        // aguarda 5 segundos para a simulação rodar e reinicia o ciclo ouvindo apenas o site
            if (modo_auto) {
            this_thread::sleep_for(chrono::seconds(5));
            ciclo++;
            continue; 
        }

        cout << "\nComando (ENTER para avancar): ";
        string cmd;
        cin.ignore();
        getline(cin, cmd);

        //se o usuário digitar 'p' no terminal, ativa o piloto automático e passa o controle para a interface web
        if (cmd == "p") {
            modo_auto = true;
            cout << "\n>>> MODO AUTOMATICO ATIVADO! <<<\n";
            ciclo++;
            continue;
        }

        if (cmd == "q") {
            cout << "Encerrando." << endl;
            break;

         } else if (cmd == "s") {
    double novaTemp, novoVol;
    cout << "Nova temperatura desejada: "; cin >> novaTemp;
    cout << "Novo volume por minuto:     "; cin >> novoVol;

    // Cria o objeto comando e executa
    // O comando já recalcula válvulas e bombas internamente
    ComandoAlterarSetPoint cmd_setpoint(novaTemp, novoVol);
    cmd_setpoint.executar(planta);

    // Registra a ação no histórico
    registro.registrarAcaoOperador(
        acesso.getNomeAtivo(),
        "Setpoint alterado via comando: " + to_string(novaTemp) +
        "C | Volume: " + to_string(novoVol) + " L/min");

        } else if (cmd == "f") {
            // Simula falha para demonstração
            cout << "Simular falha em: (1) Serpentina quente  (2) Serpentina fria: ";
            string op;
            cin >> op;
            if (op == "1") planta.simularFalhaSerpentinaQuente();
            else           planta.simularFalhaSerpentinaFria();

        } else if (cmd == "m") {
            try {
                string equip, motivo, data;
                cout << "Equipamento: ";   cin >> equip;
                cout << "Motivo: ";        cin >> motivo;
                cout << "Data prevista: "; cin >> data;
                gerManutencao.proporManutencao(equip, motivo, data, acesso);
            } catch (runtime_error& e) { cout << e.what() << endl; }

        } else if (cmd == "a") {
            try {
                string equip;
                cout << "Equipamento para aprovar: "; cin >> equip;
                gerManutencao.aprovarManutencao(equip, acesso);
            } catch (runtime_error& e) { cout << e.what() << endl; }

        } else if (cmd == "r") {
            try {
                string equip, motivo;
                cout << "Equipamento para recusar: "; cin >> equip;
                cout << "Motivo da recusa: ";         cin >> motivo;
                gerManutencao.recusarManutencao(equip, motivo, acesso);
            } catch (runtime_error& e) { cout << e.what() << endl; }

        } else if (cmd == "h") {
            try {
                gerManutencao.exibirHistorico(acesso);
            } catch (runtime_error& e) { cout << e.what() << endl; }
        }

        this_thread::sleep_for(chrono::seconds(3));
        ciclo++;
    }

    return 0;
}