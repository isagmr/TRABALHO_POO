#include "ParBombas.hpp"

ParBombas::ParBombas(string TagPrincipal, string TagReserva, double CapMax) : Principal(TagPrincipal, CapMax), Reserva(TagReserva, CapMax) { //constrói as 2 bombas de uma vez só
        UsandoReserva = false; //por padrão, a reserva vem desativada
    }

    void ParBombas::Ligar(double Potencia) {
        if (deveUsarReserva()) { //verifica se deve usar a reserva
            if(!UsandoReserva) { //testa se ja tava usando a reserva
                cout << "[REDUNDÂNCIA] Alternando para reserva: " << Reserva.getTag() << endl;
                Principal.Desligar();
                UsandoReserva = true;
            }
            Reserva.Ligar(Potencia);
        } else {
            UsandoReserva = false;
            Principal.Ligar(Potencia);
        }
    }

    void ParBombas::Desligar() { //funciona como um botão de emergencia; manda o sinal pras duas pararem
        Principal.Desligar();
        Reserva.Desligar();
    }

    double ParBombas::calcularVazao(double AberturaValvula) const {
        if (UsandoReserva) return Reserva.calcularVazao(AberturaValvula);
        return Principal.calcularVazao(AberturaValvula);
    }

    void ParBombas::AjustarPotencia(double Potencia) {
    if (UsandoReserva) Reserva.AjustarPotencia(Potencia);
    else  Principal.AjustarPotencia(Potencia);
}

    void ParBombas::atualizarMonitores(double HorasCiclo){ //a cada ciclo, avisa o tempo q passou pras duas bombas
        //mesmo a q esta desligada rece o aviso e usa esse tempo pra esfriar a temperatura
        Principal.atualizarMonitor(HorasCiclo);
        Reserva.atualizarMonitor(HorasCiclo);
    }

    //coloca a principal em manutenção e trasfere carga pra reserva
    void ParBombas::iniciarManutencaoPrincipal(SistemaAcesso& Acesso) {
        Acesso.exigirPermissao(NivelAcesso::TECNICO);
        cout << "[MANUTENÇÃO] Transferindo carga: " << Principal.getTag() << " -> " << Reserva.getTag() << endl;
        Principal.Monitor.entrarEmManutencao();
        Principal.Desligar();
        UsandoReserva = true;
    }

    //retornam uma referência ás bombas caso alguém de fora precise olhar os status delas
    Bomba& ParBombas::getPrincipal() { return Principal; }
    Bomba& ParBombas::getReserva() { return Reserva; }
    bool ParBombas::estaUsandoReserva() const { return UsandoReserva; }
    string ParBombas::getTagAtiva() const { return UsandoReserva ? Reserva.getTag() : Principal.getTag();} // essa sintaxe de ? é um operador ternário

    
    bool ParBombas::deveUsarReserva() const {
        return Principal.temFalha() || Principal.Monitor.getStatus() == StatusManutencao::MANUTENCAO;
    }