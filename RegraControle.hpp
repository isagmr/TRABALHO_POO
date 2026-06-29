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

using namespace std;

class PlantaMistura; // forward declaration — avisa que existe sem incluir tudo

class RegraControle{
    public:
    virtual ~RegraControle() = default;
    virtual void aplicar(PlantaMistura& planta) = 0; // '= 0' significa: função sem lógica aqui "{}", as classes filhas são OBRIGADAS a programar a lógica para cada classe especifica
};