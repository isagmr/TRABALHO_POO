#include "PlantaMistura.hpp"


PlantaMistura::PlantaMistura() : ResQuente("RES-Q"), ResFrio("RES-F"), Tanque("TQ-MIX"), BombasQuente("BOMBA-Q1", "BOMBA-Q2", 80.0), BombasFria("BOMBA-F1", "BOMBA-F2", 80.0), ValvulaQuente("FV-QUENTE"), ValvulaFria("FV-FRIA") {
        BombaSaidaLigada = false;
        SetpointTemperatura = 45.0;
    }

    //chamado todo ciclo pra atualizar toda a planta
    void PlantaMistura::atualizar(double HorasCiclo) {
        //atualiza os reservatorios e as serpentinas de cada um
        ResQuente.atualizar(HorasCiclo);
        ResFrio.atualizar(HorasCiclo);

        //calcula as vazoes reais com base nas bombas e valvulas
        double vazaoQuente = BombasQuente.calcularVazao(ValvulaQuente.getAbertura());
        double vazaoFria = BombasFria.calcularVazao(ValvulaFria.getAbertura());

        //atualiza o tanque com as informações do ciclo
        Tanque.atualizar(vazaoQuente, vazaoFria, ResQuente.getTemperatura(), ResFrio.getTemperatura(), BombaSaidaLigada, HorasCiclo);

        //atualiza os monitores de desgaste das bombas
        BombasQuente.atualizarMonitores(HorasCiclo);
        BombasFria.atualizarMonitores(HorasCiclo);
    }

    //aplica os parametros calculados pela calculadora demanda
    void PlantaMistura::aplicarParametros(CalculadoraDemanda::Parametros p) {
        ValvulaQuente.Abrir(p.AberturaValvulaQuente);
        ValvulaFria.Abrir(p.AberturaValvulaFria);
        BombasQuente.Ligar(p.PotenciaBombaQuente);
        BombasFria.Ligar(p.PotenciaBombaFria);
    }

    //usar pra ler o estado da planta
    double PlantaMistura::getTemperaturaMistura() const { return Tanque.getTemperatura(); }
    double PlantaMistura::getNivelTanque() const { return Tanque.getNivel(); }
    double PlantaMistura::getPressaoSaida() const { return Tanque.getPressao(); }
    double PlantaMistura::getSetpointTemperatura() const { return SetpointTemperatura; }
    double PlantaMistura::getTempReservatorioQ() const { return ResQuente.getTemperatura(); }
    double PlantaMistura::getTempReservatorioF() const { return ResFrio.getTemperatura(); }
    double PlantaMistura::getLimiteBaixo() const { return Tanque.getLimiteBaixo(); }
    double PlantaMistura::getLimiteAlto() const { return Tanque.getLimiteAlto(); }
    double PlantaMistura::getLimiteCritico() const { return Tanque.getLimiteCritico(); }
    double PlantaMistura::getLimitePressao() const { return Tanque.getLimitePressao(); }

    //ações que as regras chamam pra atuar na planta
    void PlantaMistura::aumentarAguaQuente() {
        ValvulaQuente.Abrir(ValvulaQuente.getAbertura() + 5.0);
        BombasQuente.Ligar(BombasQuente.calcularVazao(ValvulaQuente.getAbertura()) > 0 ? 70.0 : 20.0);
    }
    void PlantaMistura::reduzirAguaQuente() {
        ValvulaQuente.Abrir(ValvulaQuente.getAbertura() - 5.0);
    }
    void PlantaMistura::aumentarAguaFria() {
        ValvulaFria.Abrir(ValvulaFria.getAbertura() + 5.0);
        BombasFria.Ligar(BombasFria.calcularVazao(ValvulaFria.getAbertura()) > 0 ? 70.0 : 20.0);
    }
    void PlantaMistura::reduzirAguaFria() {
        ValvulaFria.Abrir(ValvulaFria.getAbertura() - 5.0);
    }
    void PlantaMistura::fecharEntradas() {
        ValvulaQuente.Fechar();
        ValvulaFria.Fechar();
        BombasQuente.Desligar();
        BombasFria.Desligar();
    }
    void PlantaMistura::ligarBombaSaida() { BombaSaidaLigada = true; }
    void PlantaMistura::desligarBombaSaida() { BombaSaidaLigada = false; }

    void PlantaMistura::setSetpointTemperatura(double Sp) { SetpointTemperatura = Sp;}

    void PlantaMistura::adicionarAlarme(string Codigo) {
        Alarmes.push_back(Codigo);
        cout << "[ALARME] " << Codigo << endl;
    }
    // a partir daqui recebe comandos do supervisor
    void PlantaMistura::resetarAlarmes() { Alarmes.clear(); }
    vector<string> PlantaMistura::getAlarmes() const { return Alarmes; }

    //referencias para pro gerenciador manutenção registrar os monitores
    ParBombas& PlantaMistura::getBombasQuente() { return BombasQuente; }
    ParBombas& PlantaMistura::getBombasFria() { return BombasFria; }
    Serpentina& PlantaMistura::getSerpentinaQ() { return ResQuente.getSerpentina(); }
    Serpentina& PlantaMistura::getSerpentinaF() { return ResFrio.getSerpentina(); }

    //simular a falha surante a demonstração
    void PlantaMistura::simularFalhaSerpentinaQuente() { ResQuente.simularFalhaSerpentina(); }
    void PlantaMistura::simularFalhaSerpentinaFria() { ResFrio.simularFalhaSerpentina(); }

    // NOVA FUNÇÃO: Recebe as ordens do Python e aplica na PlantaMistura
    void PlantaMistura::receberComandosSupervisorio(double novo_setpoint, bool emergencia, bool forcar_q, bool forcar_f) {
        if (emergencia) {
            cout << "\n🚨 [SUPERVISÓRIO] PARADA DE EMERGÊNCIA ACIONADA! 🚨\n";
            fecharEntradas();
            desligarBombaSaida();
            adicionarAlarme("EMERGENCIA_SUPERVISOR");
        } else {
            // Só altera se o supervisor mandar um valor de temperatura novo
            if (novo_setpoint != SetpointTemperatura) {
                // Usa a calculadora que o seu grupo criou para ajustar as válvulas!
                CalculadoraDemanda::Parametros p = CalculadoraDemanda::calcular(novo_setpoint, 2.0); 
                aplicarParametros(p);
                setSetpointTemperatura(novo_setpoint);
            }
            
            // Força as bombas usando as classes ParBombas do seu projeto
            if (forcar_q) {
                BombasQuente.getPrincipal().Ligar(100.0); 
            }
            if (forcar_f) {
                BombasFria.getPrincipal().Ligar(100.0);
            }
        }
    }