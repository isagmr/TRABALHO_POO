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


#include "ReservatorioQuente.hpp"
#include "ReservatorioFrio.hpp"
#include "TanqueMistura.hpp"
#include "ParBombas.hpp"
#include "Valvula.hpp"
#include "CalculadoraDemanda.hpp"

using namespace std;

class PlantaMistura {
    private:
    ReservatorioQuente ResQuente;
    ReservatorioFrio ResFrio;
    TanqueMistura Tanque;
    ParBombas BombasQuente;
    ParBombas BombasFria;
    Valvula ValvulaQuente;
    Valvula ValvulaFria;
    bool BombaSaidaLigada;
    double SetpointTemperatura;
    vector<string> Alarmes;

    public:
    PlantaMistura();
    void atualizar(double HorasCiclo);
    void aplicarParametros(CalculadoraDemanda::Parametros p);

    double getTemperaturaMistura() const;
    double getNivelTanque() const;
    double getPressaoSaida() const;
    double getSetpointTemperatura() const;
    double getTempReservatorioQ() const;
    double getTempReservatorioF() const;
    double getLimiteBaixo() const;
    double getLimiteAlto() const;
    double getLimiteCritico() const;
    double getLimitePressao() const;

    //ações que as regras chamam pra atuar na planta
    void aumentarAguaQuente();
    void reduzirAguaQuente();
    void aumentarAguaFria();
    void reduzirAguaFria();
    void fecharEntradas();
    void ligarBombaSaida();
    void desligarBombaSaida();

    void setSetpointTemperatura(double Sp);
    void adicionarAlarme(string Codigo);
    void resetarAlarmes();
    vector<string> getAlarmes() const;

    ParBombas& getBombasQuente();
    ParBombas& getBombasFria();
    Serpentina& getSerpentinaQ();
    Serpentina& getSerpentinaF();

    void simularFalhaSerpentinaQuente();
    void simularFalhaSerpentinaFria();

    void receberComandosSupervisorio(double novo_setpoint, bool emergencia, bool forcar_q, bool forcar_f);

};