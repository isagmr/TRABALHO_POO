#include "GerenciadorManutencao.hpp"

GerenciadorManutencao::GerenciadorManutencao(RegistroSistema& R) : Registro(R) {} //temos que passar o registro p ele nascer na main

void GerenciadorManutencao::registrarMonitor(MonitorEquipamento* M) { //adicionando as máquinas na lista do lista do gerenciador
    Monitores.push_back(M); //push back joga no final da lista
}

//Técnico propõe a manutenção e fica pendente ate o admin aprovar
//pede o nome da maquina, o pq quebrou, a data e EXIGE o acesso pra ver quem ta logado
void GerenciadorManutencao::proporManutencao(string NomeEquip, string Motivo, string Data, SistemaAcesso& Acesso) {
    Acesso.exigirPermissao(NivelAcesso::TECNICO);

    AgendamentoManutencao ag; //criando uma 'prancheta de serviço' nova e em branco
    //preeche esses dados usando os parâmetros da função
    ag.NomeEquipamento = NomeEquip;
    ag.Motivo = Motivo;
    ag.DataAgendada = Data;
    ag.NomeTecnico = Acesso.getNomeAtivo();  //puxa automaticamente os dados de quem ta logado
    //preenche o resto com dados vazios / padrões pq ainda n houve aprovação/recusa
    ag.NomeAdmin = "";
    ag.MotivoRecusa = "";
    ag.Aprovada = false;
    ag.Pendente = true;
    ag.Concluida = false;
    Agendamentos.push_back(ag); //guradndo essa prancheta dentro da lista agendamentos

    Registro.registrarPropostaManutencao(Acesso.getNomeAtivo(), NomeEquip, Motivo, Data); //escrevendo no arquivo q o tecnico pediu essa mantenção
}

//admin aprovou uma proposta pendente
void GerenciadorManutencao::aprovarManutencao(string NomeEquip, SistemaAcesso& Acesso) {
    Acesso.exigirPermissao(NivelAcesso::ADMIN);

    for (auto& ag : Agendamentos) { //abre a lista agendamentos e olha prancheta por prancheta; auto& garante q é a prancheta original e não uma cópia; o ag é a prancheta usada no momneto
        if (ag.NomeEquipamento == NomeEquip && ag.Pendente) { //testa se o nome do equipamento é o mesmo q o admin quer aprovar e se está como pendente
            //se for a certa, ele altera o status e assina o nome do admin logado
            ag.Aprovada = true;
            ag.Pendente = false;
            ag.NomeAdmin = Acesso.getNomeAtivo();

            Registro.registrarAprovacaoManutencao(Acesso.getNomeAtivo(), ag.NomeTecnico, NomeEquip);

            //avisa o monitor do equipamento
            for (auto* m : Monitores) { //olha a lista de monitores e olha máquina por maquina (m)
                //se achar a máquina com o nome certo, chama a função entrar em manutnção, que congela o desgaste e avisa ao sistema q ela parou
                if (m->getNomeEquipamento() == NomeEquip) {
                    m->entrarEmManutencao();
                }
            }
            return; //como ja achou e aprovou a manutenção, n precisa continuar olhando as pranchetas
        }
    }
    cout << "[MANUTENÇÃO] Nenhuma proposta pendente encontrada para: " << NomeEquip << endl;
}

//admin recusa uma proposta com motivo obrigatório
void GerenciadorManutencao::recusarManutencao(string NomeEquip, string MotivoRecusa, SistemaAcesso& Acesso) {
    Acesso.exigirPermissao(NivelAcesso::ADMIN);

    for (auto& ag : Agendamentos) {
        if (ag.NomeEquipamento == NomeEquip && ag.Pendente) {
            ag.Aprovada = false;
            ag.Pendente = false;
            ag.NomeAdmin = Acesso.getNomeAtivo();
            ag.MotivoRecusa = MotivoRecusa;

            Registro.registrarRecusaManutencao(Acesso.getNomeAtivo(), ag.NomeTecnico, NomeEquip, MotivoRecusa);
            return;
        }
    }
    cout << "[MANUTENÇÃO] Nenhuma proposta pendente encontrada para: " << NomeEquip << endl;
}

//tecnico conclui a manutenção após a aprovação
void GerenciadorManutencao::concluirManutencao(string NomeEquip, SistemaAcesso& Acesso) {
    Acesso.exigirPermissao(NivelAcesso::TECNICO);

    for (auto& ag : Agendamentos) {
        if (ag.NomeEquipamento == NomeEquip && ag.Aprovada && !ag.Concluida) {
            ag.Concluida = true;

            Registro.registrarConclusaoManutencao(Acesso.getNomeAtivo(), NomeEquip);

            for (auto* m : Monitores) {
                if (m->getNomeEquipamento() == NomeEquip) {
                    m->concluirManutencao();
                }
            }
            return;
        }
    }
    cout << "[MANUTENÇÃO] Nenhuma manutenção aprovada pendente para: " << NomeEquip << endl;
}

//verifica alarmes automáticos e registra no arquivo
void GerenciadorManutencao::verificarAlertas() {
    for (auto* m : Monitores) {
        if (m->getStatus() == StatusManutencao::ATENCAO) {
            Registro.registrarAlarme(m->getNomeEquipamento() + ": " + m->getMotivoAlerta() + " | Solicite manutenção ao técnico.");
        }
    }
}

//exibe só pro admin; essa função é proibida de acessar qualquer variavel, só pode ler, por isso o const
void GerenciadorManutencao::exibirHistorico(SistemaAcesso& Acesso) const {
    Acesso.exigirPermissao(NivelAcesso::ADMIN);
    cout << "\n-- HISTÓRICO DE MANUTENÇÕES --" << endl;

    if (Agendamentos.empty()) { cout << " Nenhum registro." << endl; return; } //testa se a lista ta completamente vazia

    for (const auto& ag : Agendamentos) {
        cout << " Equipamento: " << ag.NomeEquipamento << " | Tecnico: " << ag.NomeTecnico << " | Motivo: " << ag.Motivo << " | Data: " << ag.DataAgendada;
        if (ag.Pendente) {
            cout << " | AGUARDANDO APROVAÇÃO";
        } else if (ag.Aprovada) {
            cout << " | APROVADA por: " << ag.NomeAdmin;
            if (ag.Concluida) cout << " | CONCLUÍDA";
        } else {
            cout << " | RECUSADA por: " << ag.NomeAdmin << " | Motivo: " << ag.MotivoRecusa;
        }
        cout << endl;
    }
}