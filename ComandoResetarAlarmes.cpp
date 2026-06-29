#include "ComandoResetarAlarmes.hpp"
#include "PlantaMistura.hpp"
#include "Comando.hpp"

void ComandoResetarAlarmes::executar(PlantaMistura& planta) {
        planta.resetarAlarmes();
        cout << "[COMANDO] Alarmes resetados." << endl;
    }

    string ComandoResetarAlarmes::nome() const {
        return "RESETAR_ALARMES";
    }