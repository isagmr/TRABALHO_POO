#include "RegraPressaoAlta.hpp"
#include "PlantaMistura.hpp"
#include "RegraFalhaReservatorio.hpp"  // com .hpp
#include "ComandoAlterarSetPoint.hpp"


void RegraPressaoAlta::aplicar(PlantaMistura& planta) {
        if (planta.getPressaoSaida() >= planta.getLimitePressao()) {
            planta.fecharEntradas();
            planta.adicionarAlarme("PRESSAO_ALTA");
        }
    }