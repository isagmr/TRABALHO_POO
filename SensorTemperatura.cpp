#include "SensorTemperatura.hpp"
#include "Sensor.hpp"
#include "TipoSensorTemp.hpp"

SensorTemperatura::SensorTemperatura(string T, TipoSensorTemp Tp, double ValorInicial) : Sensor(T, "°C") { //construtor
        Tipo = Tp;
        Valor = ValorInicial;
   }
   
   //simula leitura do reservatório quente ou frio com pequeno ruído; a serpentina vai manter o valor perto do alvo, então o ruído é pequeno
   void SensorTemperatura::simular(bool BombaLigada, bool ValvulaAberta) { //override avisa ao compilador q estamos reescrevendo a função simular q já existia na classe mãe; recebe o estado das bombas e válvulas só pq a classe mãe exige, mas não usamos
        static random_device rd;
        static mt19937 gen(rd());

        //reservatorios tem o ruido pequeno pq a serpentina estabiliza
        if (Tipo == TipoSensorTemp::RESERVATORIO_QUENTE || Tipo == TipoSensorTemp::RESERVATORIO_FRIO) {
            uniform_real_distribution<double> ruido(-0.3, 0.3); //cria um sorteio de numeros quebrados entre -0,3 e 0,3 graus
            Valor += ruido(gen); //simula o ruido eletrico adicionando os valores sorteados antes; o sensor nunca vai medir a temperatura perfeitamente estatica, sempre vai ter uma pequena oscilação
        }
   }

   //o tanque de mistura é atualizado pelo atualizarValorMistura()
   void SensorTemperatura::atualizarValorMistura(double NovaTemperatura) {
    Valor = NovaTemperatura;
   }

   //aplica o efeito da serpentina no reservatorio
   void SensorTemperatura::atualizarValorReservatorio(double VariacaoSerpentina, double TempAlvo) {
    Valor += VariacaoSerpentina;
    //simula q o liquido resiste a mudanças bruscas de temperatura
    if (Tipo == TipoSensorTemp::RESERVATORIO_QUENTE) {
        if (Valor > 75.0) Valor = 75.0; //limite max de segurança
        if (Valor < 55.0) Valor = 55.0; //minimo: caldeira sempre ajuda
    } else if (Tipo == TipoSensorTemp::RESERVATORIO_FRIO) {
        if (Valor > 30.0) Valor = 30.0;
        if (Valor < 10.0) Valor = 10.0;
    }
   }

   TipoSensorTemp SensorTemperatura::getTipo() const { return Tipo; } //entrega qual a utilidade e o tipo do sensor