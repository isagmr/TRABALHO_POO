#include "SensorPressao.hpp"
#include "Sensor.hpp"

SensorPressao::SensorPressao(string t) : Sensor(t, "BAR"){
        Valor = 0.0;
    }
    void SensorPressao::simular (bool BombaLigada, bool AbrirValvula) {

        static std::random_device rd;
	    static std::mt19937 gen(rd());

        if(BombaLigada == true){   // if(BombaLigada)
            // 2. Definir que a variação vai ser pequena: entre [0.3% e 0.8%] 
	        std::uniform_real_distribution<double> variacao(0.3, 0.8);
            // 3. Aplicar a pequena variação no valor atual
	        Valor += variacao(gen);
            // 4. Segurança: não deixa a pressão passsar de 10
	        if(Valor > 10.0){
		    Valor = 10.0;
	        }
        }

	    else if(BombaLigada == false){    // if(!BombaLigada)
            // 2. Definir que a variação vai ser pequena: entre [-0.3% e -0.8%] 
	        std::uniform_real_distribution<double> variacao(-0.8, -0.3);
            // 3. Aplicar a pequena variação no valor atual
	        Valor += variacao(gen);
            // 4. Segurança: não deixa a pressão ficar negativa (< 0.0%)
	        if(Valor < 0.0){
		    Valor = 0.0;
	        }    
        }
    }