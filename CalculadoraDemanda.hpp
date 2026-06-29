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


class CalculadoraDemanda{
    public:
    struct Parametros {
    double AberturaValvulaQuente;
    double AberturaValvulaFria;
    double PotenciaBombaQuente;
    double PotenciaBombaFria;
};

    //guarda o resultado do cálculo - tudo varia de 0 a 100%
    struct Parametros;

    //recebe o que o operador quer e devolve o que o sistema deve fazer; temDesejada é a temperatura em °C que o operador pediu e volumePorMinuto é a vazão de saída desejada em l/min
    static Parametros calcular(double TempDesejada, double VolumePorMinuto);

};