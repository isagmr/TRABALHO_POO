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

#include "MonitorEquipamento.hpp"
#include "StatusManutencao.hpp"

using namespace std;

class Bomba {
    private:
    string Tag;
    bool Ligada;
    bool Falha;
    double Potencia; //varia de  a 100%
    double CapacidadeMaxima; //litros por minuto a 100% de potência e válvula 100% aberta
    bool EstavaLigada; //detecta transição desligada -> ligada

    public:
    MonitorEquipamento Monitor;

    Bomba(string T, double CapMax);
    void Ligar(double PotenciaDesejada);
    void AjustarPotencia(double NovaPotencia);
    void Desligar();
    void AtivarFalha();
    void atualizarMonitor(double HorasCiclo);
    double calcularVazao(double AberturaValvula) const;

    bool estaLigada() const;
    bool temFalha() const;
    double getPotencia() const;
    double getCapacidadeMaxima() const;
    string getTag() const;

};