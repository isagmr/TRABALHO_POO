#include "SensorNivel.hpp"
#include "Sensor.hpp"

SensorNivel::SensorNivel(string t) : Sensor(t, "%") {
        Valor = 90.0;
    } 
    // Para a função simular vamos tentar usar o conceito chamado "Caminhada Aleatória (Random Walk)"
    // Em vez de sortear números completamente novos e aleatórios pegamos o 'valor' e sorteamos uma pequana variação (um "ruido") para subtrair dele
    void SensorNivel::simular(bool BombaLigada, bool ValvulaAberta) { 

        // 1. Configurar o gerador de números aleatórios do C++
        static std::random_device rd;
	    static std::mt19937 gen(rd());

        if(BombaLigada == true){ // Se a bomba estiver ligada 
            
            // 2. Definir que a variação vai ser pequena: entre [0.5% e 1.5%] 
	        std::uniform_real_distribution<double> variacao(0.5, 1.5);
            // 3. Aplicar a pequena variação no valor atual
	        Valor += variacao(gen);
            // 4. Segurança: não deixa o reservatório passsar do 100% nem ficar negativo (< 0.0%)
	        if(Valor > 100.0){
		    Valor = 100.0;
	        }
	        if(Valor < 0.0){
		    Valor = 0.0;
	        }    
        }

        else if(ValvulaAberta == true){
            
            // 2. Definir que a variação vai ser pequena: entre [-0.5% e -1.5%] 
	        std::uniform_real_distribution<double> variacao(-1.5, -0.5);
            // 3. Aplicar a pequena variação no valor atual
	        Valor += variacao(gen);
            // 4. Segurança: não deixa o reservatório passsar do 100% nem ficar negativo (< 0.0%)
	        if(Valor > 100.0){
		    Valor = 100.0;
	        }
	        if(Valor < 0.0){
		    Valor = 0.0;
	        } 
        }

        else{
            

            // 2. Definir que a variação vai ser pequena: entre [-1.5% e 1.5%] 
	        std::uniform_real_distribution<double> variacao(-1.5, 1.5);

            // 3. Aplicar a pequena variação no valor atual
	        Valor += variacao(gen);

            // 4. Segurança: não deixa o reservatório passsar do 100% nem ficar negativo (< 0.0%)
	        if(Valor > 100.0){
		        Valor = 100.0;
	        }
	        if(Valor < 0.0){
		        Valor = 0.0;
	        }
        }
    }