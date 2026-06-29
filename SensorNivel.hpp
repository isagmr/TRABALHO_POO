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
#include "Sensor.hpp"

using namespace std;

class SensorNivel : public Sensor {
    public:
    // Chama o construtor e
    // 'Empurra' o texto "t" para a classe "mãe" e avisa que a unidade de medida utilizada nesta classe será %
    SensorNivel(string t);
    void simular(bool BombaLigada, bool ValvulaAberta) override;
};