#include "RegraNivelBaixo.hpp"
#include "PlantaMistura.hpp"
#include "RegraFalhaReservatorio.hpp"  // com .hpp
#include "ComandoAlterarSetPoint.hpp"

void RegraNivelBaixo::aplicar(PlantaMistura& planta) {
        if (planta.getNivelTanque() <= planta.getLimiteBaixo()) {
            planta.aumentarAguaQuente();
            planta.aumentarAguaFria();
            planta.adicionarAlarme("NIVEL_BAIXO");
        }
    }