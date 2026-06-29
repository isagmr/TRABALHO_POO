#include "RegraControleTemperatura.hpp"
#include "PlantaMistura.hpp"

void RegraControleTemperatura::aplicar(PlantaMistura& planta) {
        double temp     = planta.getTemperaturaMistura();
        double setpoint = planta.getSetpointTemperatura();
        double tolerancia = 1.0;

        if (temp < setpoint - tolerancia) {
            // Temperatura baixa: abre mais quente, reduz fria
            planta.aumentarAguaQuente();
            planta.reduzirAguaFria();
        } else if (temp > setpoint + tolerancia) {
            // Temperatura alta: abre mais fria, reduz quente
            planta.aumentarAguaFria();
            planta.reduzirAguaQuente();
        }
    }