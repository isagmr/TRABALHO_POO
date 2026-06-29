#include "ReservatorioFrio.hpp"
#include "Serpentina.hpp"
#include "SensorTemperatura.hpp"

    ReservatorioFrio::ReservatorioFrio(string T) : Tag(T), SerpentinaResfriamento(T + "-SERP", TipoSerpentina::RESFRIAMENTO, 20.0), Sensor(T + "-TT", TipoSensorTemp::RESERVATORIO_FRIO, 20.0) {
        SerpentinaResfriamento.Ligar(50.0);
    }

    void ReservatorioFrio::atualizar(double HorasCiclo) {
        double tempAtual = Sensor.getValor();

        if (tempAtual > TEMP_ALVO + TOLERANCIA) {
            //temp subiu - aumenta potencia de resfriamento
            double novaPotencia = SerpentinaResfriamento.getPotencia() + 10.0;
            SerpentinaResfriamento.AjustarPotencia(novaPotencia);
        } else if (tempAtual < TEMP_ALVO - TOLERANCIA) {
            //temp caiu - reduz potencia
            double novaPotencia  = SerpentinaResfriamento.getPotencia() - 10.0;
            SerpentinaResfriamento.AjustarPotencia(novaPotencia);
        }

        double variacao = SerpentinaResfriamento.calcularVariacaoTemperatura();
        Sensor.atualizarValorReservatorio(variacao, TEMP_ALVO);
        Sensor.simular(false, false);

        SerpentinaResfriamento.atualizarMonitor(HorasCiclo);
    }

    void ReservatorioFrio::simularFalhaSerpentina() {
        SerpentinaResfriamento.AtivarFalha();
        cout << "[FALHA] Serpentina do reservatório frio falhou!" << endl;
    }

    double ReservatorioFrio::getTemperatura()     const { return Sensor.getValor(); }
    string ReservatorioFrio::getTagSensor()       const { return Sensor.getTag(); }
    Serpentina& ReservatorioFrio::getSerpentina()       { return SerpentinaResfriamento; }
    SensorTemperatura& ReservatorioFrio::getSensor()    { return Sensor; }