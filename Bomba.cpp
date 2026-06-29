#include "Bomba.hpp"

Bomba::Bomba(string T, double CapMax) : Monitor(T) {
        Tag = T;
        Ligada = false;
        Falha = false;
        Potencia = 0.0;
        CapacidadeMaxima = CapMax;
        EstavaLigada = false;
    }

void Bomba::Ligar(double PotenciaDesejada){
        if (Falha == true){
            return; // com defeito não liga
        }
        if (Monitor.getStatus() == StatusManutencao::MANUTENCAO) {
            cout << "[AVISO] " << Tag << " está em manutenção." << endl;
            return;
        }
        if (!EstavaLigada) Monitor.registrarCicloLigaDesliga();
        Ligada = true;
        EstavaLigada = true;
        Potencia = PotenciaDesejada;
        if (Potencia > 100.0) Potencia = 100.0;
        if (Potencia < 0.0) Potencia = 0.0;
    }

    void Bomba::AjustarPotencia(double NovaPotencia){
        if (Falha == true || Ligada == false){
            return; // só ajusta se estiver ligada e sem falha
        }
        Potencia = NovaPotencia;
        if (Potencia > 100.0) Potencia = 100.0;
        if (Potencia < 0.0)   Potencia = 0.0;
    }

    void Bomba::Desligar(){
        EstavaLigada = false;
        Ligada = false;
        Potencia = 0.0;
    }

    void Bomba::AtivarFalha(){
        Falha = true;
        Ligada = false;
        Potencia = 0.0;
        EstavaLigada = false;
    }

    void Bomba::atualizarMonitor(double HorasCiclo) {
        Monitor.atualizar(Ligada, Potencia, HorasCiclo);
    }

    //aqui retornamos a vazão real que a bomba está produzindo, e pra calcular completamente, a válvula precisa passar sua abertura
    double Bomba::calcularVazao(double AberturaValvula) const {
        if (Ligada == false || Falha == true){
            return 0.0;
        }
        return (Potencia / 100.0) * (AberturaValvula / 100.0) * CapacidadeMaxima;
    }

    bool Bomba::estaLigada() const { return Ligada; }
    bool Bomba::temFalha() const { return Falha; }
    double Bomba::getPotencia() const { return Potencia; }
    double Bomba::getCapacidadeMaxima() const { return CapacidadeMaxima; }
    string Bomba::getTag() const { return Tag; }