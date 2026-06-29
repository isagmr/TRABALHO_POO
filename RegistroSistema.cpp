#include "RegistroSistema.hpp"
#include "RegraFalhaReservatorio.hpp"  // com .hpp
#include "ComandoAlterarSetPoint.hpp"

RegistroSistema::RegistroSistema() {
        CicloAtual = 0;

        ArquivoAlarmes.open("historico_alarmes.txt", ios::app); //ios::app significa que novas entradas são adicionadas ao fim do arquivo sem apagar oq já estava salvo
        ArquivoAcoes.open("historico_acoes.txt", ios::app);

        //para marcar uma nova sessão nos arquivos sempre que houver
        ArquivoAlarmes << "\n=== NOVA SESSAO ===" << endl;
        ArquivoAcoes << "\n=== NOVA SESSAO ===" << endl;
    }

RegistroSistema::~RegistroSistema() {     //destrutor da classe, garante que os arquivos serão fechados corretamente (segurança - não corromper)
        ArquivoAlarmes.close();
        ArquivoAcoes.close();
    }

//variável recebe o numero do ciclo e está protegida por encapsulamento de modo que não da pra alterar ela pela main, ou seja
//o valor do ciclo segue o fluxo do processo
void RegistroSistema::setCiclo(int C) {
        CicloAtual = C; 
    }

    //registra alarme no arquivo e exibe no terminal
    //usando concatenação de strings (somando textos); o + faz uma 'fusão' de textos; a função to_string transforma o numero do ciclo em texto
void RegistroSistema::registrarAlarme(string Descricao){ //a descrição já é uma string q veio do parametro da função
        string linha = "[CICLO " + to_string(CicloAtual) + "] ALARME: " + Descricao;
        cout << linha << endl; //envia pro terminal
        ArquivoAlarmes << linha << endl; //envia pro arquivo
    }

//registra um alarme rconhecido: quem reconheceu fica salvo
void RegistroSistema::registrarReconhecimentoAlarme(string Alarme, string NomeUsuario) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] ALARME RECONHECIDO: " + Alarme + " | Por: " + NomeUsuario;
        cout << linha << endl;
        ArquivoAlarmes << linha << endl;
    }

//registra qualquer ação do operador
void RegistroSistema::registrarAcaoOperador(string NomeUsuario, string Acao) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] OPERADOR " + NomeUsuario + ": " + Acao;
        cout << linha << endl;
        ArquivoAcoes << linha << endl;
    }

//registra as propostas de manutenções feitas pelo técnico
void RegistroSistema::registrarPropostaManutencao(string NomeTecnico, string Equipamento, string Motivo, string Data) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] PROPOSTA DE MANUTENÇÃO" + " | Técnico: " + NomeTecnico + " | Equipamento " + Equipamento + " | Motivo: " + Motivo + " | Data: " + Data + " | Status: AGUARDANDO APROVAÇÃO";
        cout << linha << endl;
        ArquivoAcoes << linha << endl;
    }

//registra aprovação de manutenção pelo admin
void RegistroSistema::registrarAprovacaoManutencao(string NomeAdmin, string NomeTecnico, string Equipamento) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] MANUTENÇÃO APROVADA" + " | Admin: " + NomeAdmin + " | Técnico: " + NomeTecnico + " | Equipamento " + Equipamento;
        cout << linha << endl;
        ArquivoAcoes << linha << endl;
    }

//registra recusa de manutenção pelo admin com motivo obrigatório
void RegistroSistema::registrarRecusaManutencao(string NomeAdmin, string NomeTecnico, string Equipamento, string MotivoRecusa) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] MANUTENÇÃO RECUSADA" + " | Admin: " + NomeAdmin + " | Técnico: " + NomeTecnico + " | Equipamento " + Equipamento + " | Motivo: " + MotivoRecusa;
        cout << linha << endl;
        ArquivoAcoes << linha << endl;
    }

//registra conclusão de manutenção
void RegistroSistema::registrarConclusaoManutencao(string NomeTecnico, string Equipamento) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] MANUTENÇÃO CONCLUÍDA" + " | Técnico: " + NomeTecnico + " | Equipamento " + Equipamento;
        cout << linha << endl;
        ArquivoAcoes << linha << endl;
    }