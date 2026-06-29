#include "ComandoLigarBombaSaida.hpp"
#include "PlantaMistura.hpp"
#include "Comando.hpp"

void ComandoLigarBombaSaida::executar(PlantaMistura& planta) {
        if(planta.getNivelTanque() <= planta.getLimiteBaixo()) {
            planta.adicionarAlarme("BLOQUEIO_BOMBA_SAIDA: nivel insuficiente");
            cout << "[COMANDO] Bomba de saida bloqueada: nível baixo." << endl;
            return;
        }
        planta.ligarBombaSaida();
        cout << "[COMANDO] Bomba de saida ligada." << endl;
    }

    string ComandoLigarBombaSaida::nome() const {
        return "LIGAR_BOMBA_SAIDA";
    }