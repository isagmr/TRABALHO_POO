#include "ComandoDesligarTudo.hpp"
#include "PlantaMistura.hpp"
#include "Comando.hpp"

void ComandoDesligarTudo::executar(PlantaMistura& planta) {
        planta.fecharEntradas();
        planta.desligarBombaSaida();
        planta.adicionarAlarme("PARADA_MANUAL_OPERADOR");
        cout << " [COMANDO] Parada manual executada." << endl;
    }

    string ComandoDesligarTudo::nome() const {
        return "DESLIGAR_TUDO";
    }