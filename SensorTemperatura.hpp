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

#include "TipoSensorTemp.hpp"
#include "Sensor.hpp"

using namespace std;

class SensorTemperatura : public Sensor { //herda a classe mãe Sensor; ganha automaticamente todas as variáveis e funções que a classe mãe tiver
    private:
    TipoSensorTemp Tipo; //guarda o tipo do sensor baseado no enum

    public:
    SensorTemperatura(string T, TipoSensorTemp Tp, double ValorInicial);
    void simular(bool BombaLigada, bool ValvulaAberta) override;
    void atualizarValorMistura(double NovaTemperatura);
    void atualizarValorReservatorio(double VariacaoSerpentina, double TempAlvo);
    TipoSensorTemp getTipo() const;
};