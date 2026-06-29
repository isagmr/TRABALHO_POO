#pragma once
#include <iostream>
#include <string>
#include <random> // Biblioteca para gerar números aleatórios (o Random Walk do código)
#include <thread> // Biblioteca que deixa pausar a execução do programa
#include <chrono> // Biblioteca para utilizar unidade de tempo
#include <vector> // para listas de usuários e agendamentos de manutenções
#include <stdexcept> // para lançar exceções de acesso negado
#include <fstream> // salvar histórico em arquivo
#include <ctime> //gerar o timestamp real de cada leitura (o timestamp amarra o dado fisico ao tempo real, pra que o operador saiba exatamente quando aconteceu o evento)

#include "AgendamentoManutencao.hpp"
#include "MonitorEquipamento.hpp"
#include "RegistroSistema.hpp"
#include "SistemaAcesso.hpp"
#include "StatusManutencao.hpp"
#include <vector>

using namespace std;

class GerenciadorManutencao {
    private:
    vector<AgendamentoManutencao> Agendamentos; //guarda os pedidos de manutenção
    vector<MonitorEquipamento*> Monitores; //pega o endereço das peças pra avisar q elas devem parar de rodar pra entrar em manutenção
    RegistroSistema& Registro; //salva no arquivo (não cria um novo, ele usa o que já foi aberto na main)

    public:
    GerenciadorManutencao(RegistroSistema& R);
    void registrarMonitor(MonitorEquipamento* M);
    void proporManutencao(string NomeEquip, string Motivo, string Data, SistemaAcesso& Acesso);
    void aprovarManutencao(string NomeEquip, SistemaAcesso& Acesso);
    void recusarManutencao(string NomeEquip, string MotivoRecusa, SistemaAcesso& Acesso);
    void concluirManutencao(string NomeEquip, SistemaAcesso& Acesso);
    void verificarAlertas();
    void exibirHistorico(SistemaAcesso& Acesso) const;
};