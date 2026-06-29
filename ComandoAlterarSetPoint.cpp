#include "ComandoAlterarSetPoint.hpp"
#include "PlantaMistura.hpp"

ComandoAlterarSetPoint::ComandoAlterarSetPoint(double Temp, double Volume) {
        NovaTemp = Temp;
        NovoVolume = Volume;
    }

void ComandoAlterarSetPoint::executar(PlantaMistura& planta) {
        CalculadoraDemanda::Parametros p = CalculadoraDemanda::calcular(NovaTemp, NovoVolume);
        planta.aplicarParametros(p);
        planta.setSetpointTemperatura(NovaTemp);
        cout << "[COMANDO] Setpoint alterado para " << NovaTemp << "C | Volume: " << NovoVolume << " L/min" << endl;
    }

    string ComandoAlterarSetPoint::nome() const {
        return "ALTERAR_SETPOINT";
    }