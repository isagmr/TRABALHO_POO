#pragma once
#include <string>
using namespace std;

struct AgendamentoManutencao {
    string NomeEquipamento;
    string Motivo;
    string DataAgendada;
    string NomeTecnico;
    string NomeAdmin;
    string MotivoRecusa;
    bool Aprovada;
    bool Pendente;
    bool Concluida;
};