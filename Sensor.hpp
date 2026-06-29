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

class Sensor {
    protected: // Assim a main não altera nada mas os sensores que herdarem a classe pode realizar as alterações necessárias.
    string Tag;
    string Unidade;
    double Valor;

    public: 
    Sensor(string t, string u);
    virtual ~Sensor();
    virtual void simular(bool BombaLigada, bool ValvulaAberta) = 0;

    string getTag() const;
    string getUnidade() const;
    double getValor() const;
};