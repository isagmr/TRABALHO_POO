#include "RegraNivelAlto.hpp"
#include "PlantaMistura.hpp"

void RegraNivelAlto::aplicar(PlantaMistura& planta) {
        if (planta.getNivelTanque() >= planta.getLimiteCritico()) {
            planta.fecharEntradas();
            planta.adicionarAlarme("NIVEL_CRITICO_TRANSBORDO");
        } else if (planta.getNivelTanque() >= planta.getLimiteAlto()) {
            planta.reduzirAguaQuente();
            planta.reduzirAguaFria();
            planta.adicionarAlarme("NIVEL_ALTO");
        }
    }