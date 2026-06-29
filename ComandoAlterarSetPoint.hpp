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

#include "Comando.hpp"
#include "PlantaMistura.hpp"
#include "CalculadoraDemanda.hpp"
using namespace std;

class ComandoAlterarSetPoint : public Comando { //herda de comando, então precisa implementar a função executar() e nome()
    private:
    double NovaTemp;
    double NovoVolume;

    public:
    ComandoAlterarSetPoint(double Temp, double Volume);
    void executar(PlantaMistura& planta) override;
    string nome() const override;
};