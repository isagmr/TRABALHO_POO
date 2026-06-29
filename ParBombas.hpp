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

#include "Bomba.hpp"
#include "SistemaAcesso.hpp"

using namespace std;

class ParBombas{
    private:
    Bomba Principal; //dois objetos bomba guardados dentro da classe
    Bomba Reserva;
    bool UsandoReserva;

    public:
    ParBombas(string TagPrincipal, string TagReserva, double CapMax);
    void Ligar(double Potencia);
    void Desligar();
    double calcularVazao(double AberturaValvula) const;
    void AjustarPotencia(double Potencia);
    void atualizarMonitores(double HorasCiclo);
    void iniciarManutencaoPrincipal(SistemaAcesso& Acesso);

    Bomba& getPrincipal();
    Bomba& getReserva();
    bool estaUsandoReserva() const;
    string getTagAtiva() const;

    private:
    bool deveUsarReserva() const;
};