#include "PlantaMistura.hpp"
#include "RegraFalhaReservatorio.hpp"  

void RegraFalhaReservatorio::aplicar(PlantaMistura& planta) {
        if (planta.getTempReservatorioQ() < 55.0) {
            planta.adicionarAlarme("FALHA_SERPENTINA_QUENTE: temperatura baixa no reservatorio quente");
        }
        if (planta.getTempReservatorioF() > 30.0) {
            planta.adicionarAlarme("FALHA_SERPENTINA_FRIA: temperatura alta no reservatorio frio");
        }
    }