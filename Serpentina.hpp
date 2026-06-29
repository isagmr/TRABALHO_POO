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

#include "TipoSerpentina.hpp"
#include "MonitorEquipamento.hpp"

using namespace std;

class Serpentina {
    private:
    string Tag;
    TipoSerpentina Tipo;
    double Potencia; //0 a 100%
    bool Ligada;
    bool Falha;
    double TemperaturaAlvo; //temperatura que ela tenta manter

    public:
    MonitorEquipamento Monitor; //mesmo sistema de desgaste das bombas; como está no bloco public, o GerenciadorManutencao vai conseguir ler e alterar a saúde  da serpentina

    Serpentina(string T, TipoSerpentina Tp, double TempAlvo);
    void Ligar(double PotenciaDesejada);
    void AjustarPotencia(double NovaPotencia);
    void Desligar();
    void AtivarFalha();
    double calcularVariacaoTemperatura() const;
    void atualizarMonitor(double HorasCiclo);

    bool estaLigada() const;
    bool temFalha() const;
    double getPotencia() const;
    double getTemperaturaAlvo() const;
    string getTag() const;
    TipoSerpentina getTipo() const;

};