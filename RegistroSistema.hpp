#pragma once
#include <string>
#include <fstream>

using namespace std;

class RegistroSistema {
    private:
    ofstream ArquivoAlarmes;
    ofstream ArquivoAcoes;
    int CicloAtual;

    public:
    RegistroSistema();
    ~RegistroSistema();
    void setCiclo(int C);
    void registrarAlarme(string Descricao);
    void registrarReconhecimentoAlarme(string Alarme, string NomeUsuario);
    void registrarAcaoOperador(string NomeUsuario, string Acao);
    void registrarPropostaManutencao(string NomeTecnico, string Equipamento, string Motivo, string Data);
    void registrarAprovacaoManutencao(string NomeAdmin, string NomeTecnico, string Equipamento);
    void registrarRecusaManutencao(string NomeAdmin, string NomeTecnico, string Equipamento, string MotivoRecusa);
    void registrarConclusaoManutencao(string NomeTecnico, string Equipamento);
};