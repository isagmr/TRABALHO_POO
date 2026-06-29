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

#include "StatusManutencao.hpp"

using namespace std;

class MonitorEquipamento {
    private:
    //só o sistema atualiza essas informações
    string NomeEquipamento;
    double HorasOperacao;
    double HorasEmPotenciaAlta;
    double TemperaturaOperacao;
    int CiclosLigaDesliga;
    StatusManutencao Status;

    //constantes de alarme (limites de segurança)
    const double LIMITE_HORAS_TOTAL = 2000.0;
    const double LIMITE_HORAS_FORCA = 500.0;
    const double LIMITE_TEMPERATURA = 80.0;
    const int LIMITE_CICLOS = 1000;

    public:
    MonitorEquipamento(string Nome);
    void atualizar(bool Ligada, double Potencia, double HorasCiclo);
    void registrarCicloLigaDesliga();
    void entrarEmManutencao();
    void concluirManutencao();

    StatusManutencao getStatus() const;
    double getHorasOperacao() const;
    double getHorasEmPotenciaAlta() const;
    double getTemperaturaOperacao() const;
    int getCiclosLigaDesliga() const;
    string getNomeEquipamento() const; 

    string getStatusTexto() const;

    string getMotivoAlerta() const;

    private: // é privado pq só o próprio monitor usa
    void verificarLimites();
};