#include "MonitorEquipamento.hpp"
#include "StatusManutencao.hpp"


MonitorEquipamento::MonitorEquipamento(string Nome) { //construtor - inicia tudo
    NomeEquipamento = Nome;
    HorasOperacao = 0.0;
    HorasEmPotenciaAlta = 0.0;
    TemperaturaOperacao = 25.0;
    CiclosLigaDesliga = 0;
    Status = StatusManutencao::NORMAL;
}

void MonitorEquipamento::atualizar(bool Ligada, double Potencia, double HorasCiclo) { //calcula o desgaste a cada ciclo; recebe 3 informações: a peça ta ligada? qual a potencia? quanto tempo passou?
    if (!Ligada) {
        if (TemperaturaOperacao > 25.0) TemperaturaOperacao -= 0.5; //se ta desligado e a peça ta quente, esfria um pouco
        return;
    }

    HorasOperacao += HorasCiclo; // soma o tempo que passou no total de horas trabalhadas

    if (Potencia > 80.0) { //desgaste por potencia
        HorasEmPotenciaAlta += HorasCiclo;
        TemperaturaOperacao += (Potencia/100.0)*0.3; //se a potencia for alta, o motor ta sofrendo; soma tempo na variavel e a temperatura sobe mais rapido
    } else {
            TemperaturaOperacao += (Potencia/100.0)*0.1; //se n for potencia alta, atemperatura sobe mais devagar
        if (TemperaturaOperacao > 60.0) TemperaturaOperacao -= 0.2; // se passar de 60 graus a refrigeração natural abaixa um pouco a temperatura
    }

    if (TemperaturaOperacao > 95.0) TemperaturaOperacao = 95.0; //impede que a temperatura passe de 95 na simulação

    verificarLimites(); //verifica se alguma soma estourou os limites de alerta
}    

void MonitorEquipamento::registrarCicloLigaDesliga() { //toda vez q a bomba for ligada ele soma 1 nos ciclos e checa os limites
    CiclosLigaDesliga++;
    verificarLimites();
}

void MonitorEquipamento::entrarEmManutencao() { //se o tecnico chamar essa função, o status muda pra manutenção e o equipamento para
    Status = StatusManutencao::MANUTENCAO;
}

void MonitorEquipamento::concluirManutencao() { //o tecnico terminou o serviço; o status volta pra NORMAL e as horas de desgaste pesado e os ciclos zeram e a temperatura volta a 25 graus, como se tivessem trocado o óleo e limpado o motor
    //as horas totais não zeram pq a peça continua sendo a mesma
    Status = StatusManutencao::NORMAL;
    HorasEmPotenciaAlta = 0.0;
    CiclosLigaDesliga = 0;
    TemperaturaOperacao = 25.0;
}

StatusManutencao MonitorEquipamento::getStatus() const { return Status; }
double MonitorEquipamento::getHorasOperacao() const { return HorasOperacao; }
double MonitorEquipamento::getHorasEmPotenciaAlta() const { return HorasEmPotenciaAlta; }
double MonitorEquipamento::getTemperaturaOperacao() const { return TemperaturaOperacao; }
int MonitorEquipamento::getCiclosLigaDesliga() const { return CiclosLigaDesliga; }
string MonitorEquipamento::getNomeEquipamento() const { return NomeEquipamento; }

string MonitorEquipamento::getStatusTexto() const {
    if (Status == StatusManutencao::MANUTENCAO) return "EM MANUTENÇÃO";
    if (Status == StatusManutencao::ATENCAO) return "ATENÇÃO";
    return "NORMAL";
}

string MonitorEquipamento::getMotivoAlerta() const {
    if (HorasOperacao >= LIMITE_HORAS_TOTAL) return "Limite de horas de operação atingido";
    if (HorasEmPotenciaAlta >= LIMITE_HORAS_FORCA) return "Excesso de horas em alta potência";
    if (TemperaturaOperacao >= LIMITE_TEMPERATURA) return "Temperatura de operação elevada";
    if (CiclosLigaDesliga >= LIMITE_CICLOS) return "Excesso de ciclos liga/desliga";
    return "";
}

void MonitorEquipamento::verificarLimites() {
    if (Status == StatusManutencao::MANUTENCAO) return;
    bool alertar = (HorasOperacao >= LIMITE_HORAS_TOTAL) || (HorasEmPotenciaAlta >= LIMITE_HORAS_FORCA) || (TemperaturaOperacao >= LIMITE_TEMPERATURA) || (CiclosLigaDesliga >= LIMITE_CICLOS);
    if (alertar) Status = StatusManutencao::ATENCAO;
}