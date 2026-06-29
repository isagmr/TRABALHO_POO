#include "SensorVazao.hpp"
#include "Sensor.hpp"

SensorVazao::SensorVazao(string t) : Sensor(t, "L/min") {
        Valor = 0.0;
    }

    //recebe se a bomba está ligada e qual a abertura da valvula
    //se a bomba ta desligada: vazao 0; se ligada:: proporcional a abertura
    void SensorVazao::simular(bool BombaLigada, bool ValvulaAberta) {
        if (!BombaLigada) {
            Valor = 0.0;
            return;
        }
        //ValvulaAberta aqui representa a abertura em bool
        Valor = ValvulaAberta ? 40.0 : 0.0;
    }

    //versao real: recebe a abertura percentual da valvula
    //100% de abertura com a bomba ligada = 80 L/min
    void SensorVazao::simularComAbertura(bool BombaLigada, double AberturaValvula) {
        if (!BombaLigada) {
            Valor = 0.0;
            return;
        }
        Valor = (AberturaValvula / 100.0) * 80.0;
    }