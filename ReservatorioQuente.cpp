#include "ReservatorioQuente.hpp"

    ReservatorioQuente::ReservatorioQuente(string T) : Tag(T), SerpentinaAquecimento(T + "-SERP", TipoSerpentina::AQUECIMENTO, 70.0), Sensor(T + "-TT", TipoSensorTemp::RESERVATORIO_QUENTE, 70.0) { //adiciona o nome do sensor ao sufixo
        //liga a serpentina na potencia media ao iniciar
        SerpentinaAquecimento.Ligar(50.0);
    }

    //chamado a cada ciclo - controla a serpentina e atualiza o sensor
    void ReservatorioQuente::atualizar(double HorasCiclo) {
        double tempAtual = Sensor.getValor();

        //controle automático da serpentina
        if (tempAtual < TEMP_ALVO - TOLERANCIA) {
            //temperatura cauiu - aumenta a potencia da serpentina
            double novaPotencia = SerpentinaAquecimento.getPotencia() + 10.0;
            SerpentinaAquecimento.AjustarPotencia(novaPotencia);
        } else if (tempAtual > TEMP_ALVO + TOLERANCIA) {
            //temperatura subiu dms - reduz a potencia
            double novaPotencia = SerpentinaAquecimento.getPotencia() - 10.0;
            SerpentinaAquecimento.AjustarPotencia(novaPotencia);
        }

        //aplica o efeito da serpentina na temperatura do sensor
        double variacao = SerpentinaAquecimento.calcularVariacaoTemperatura();
        Sensor.atualizarValorReservatorio(variacao, TEMP_ALVO);
        Sensor.simular(false, false); //aplica o ruido natural

        //atualiza o monitor de desgaste da serpentina
        SerpentinaAquecimento.atualizarMonitor(HorasCiclo);
    }

    void ReservatorioQuente::simularFalhaSerpentina() { //teste pra quebrar a serpentina de propósito e ver se o sistema de alarmes funciona
        SerpentinaAquecimento.AtivarFalha();
        cout << "[FALHA] Serpentina do reservatorio quente falhou!" << endl;
    }

    double ReservatorioQuente::getTemperatura() const { return Sensor.getValor(); }
    string ReservatorioQuente::getTagSensor() const { return Sensor.getTag(); }
    Serpentina& ReservatorioQuente::getSerpentina() { return SerpentinaAquecimento; }
    SensorTemperatura& ReservatorioQuente::getSensor() { return Sensor; }