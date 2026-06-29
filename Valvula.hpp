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

class Valvula{ // Precisa ter abertura de 0 a 100%
    private:
    string Tag;
    double Abertura; // 0-100%
    bool Travada;
    
    public:
    Valvula(string T);
    void Abrir(double Percentual);
    void Fechar();
    void Travar();
    void Destravar();

    double getAbertura() const;
    bool estaTravada() const;
    string getTag() const;
};