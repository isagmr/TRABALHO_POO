#pragma once
#include <iostream>
#include <string>
#include <random> // Biblioteca para gerar números aleatórios (o Random Walk do código)
#include <thread> // Biblioteca que deixa pausar a execução do programa
#include <chrono> // Biblioteca para utilizar unidade de tempo
#include <vector> // para listas de usuários e agendamentos de manutenções
#include <stdexcept> // para lançar exceções de acesso negado
#include <fstream> // salvar histórico em arquivo
#include <ctime> //gerar o timestamp real de cada leitura (o timestamp amarra o dado fisico ao tempo real, pra que o operador saiba exatamente quando aconteceu o evento)

#include "Serpentina.hpp"
#include "SensorTemperatura.hpp"
#include "TipoSerpentina.hpp"
#include "TipoSensorTemp.hpp"

using namespace std;

class ReservatorioQuente {
    private:
    string Tag;
    Serpentina SerpentinaAquecimento;
    SensorTemperatura Sensor;

    const double TEMP_ALVO = 70.0;
    const double TOLERANCIA = 2.0; //aceita entre 68 e 72 graus

    public:
    ReservatorioQuente(string T) ;
    void atualizar(double HorasCiclo);
    void simularFalhaSerpentina();

    double getTemperatura() const;
    string getTagSensor() const;
    Serpentina& getSerpentina();
    SensorTemperatura& getSensor();
};