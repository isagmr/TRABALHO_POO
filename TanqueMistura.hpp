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

#include "SensorTemperatura.hpp"
#include "SensorNivel.hpp"
#include "SensorPressao.hpp"
#include "TipoSensorTemp.hpp"

using namespace std;

class TanqueMistura {
    private:
    string Tag;
    double Nivel;
    double Pressao;
    //3 sensores independentes monitorando o tanque
    SensorTemperatura SensorTemp;
    SensorNivel SensorNiv;
    SensorPressao SensorPress;
    //limites de segurança usados pra disparar alarmes no sistema caso o tanque mude de estado
    const double LIMITE_NIVEL_BAIXO = 27.5;
    const double LIMITE_NIVEL_ALTO = 82.5;
    const double LIMITE_NIVEL_CRITICO = 95.0;
    const double LIMITE_PRESSAO_ALTA = 6.5;

    public:
    //O tanque nasce preenchido até a metade (50%), com pressão atmosférica padrão (1.0 atm), temperatura ambiente de 23°C e gera as tags automáticas dos três sensores (-TT para temperatura, -LT para nível/líquido, -PT para pressão)
    TanqueMistura(string T) ;
    void atualizar(double VazaoQuente, double VazaoFria, double TempQuente, double TempFria, bool BombaSaidaLigada, double HorasCiclo);
    
    double getTemperatura() const;
    double getNivel() const;
    double getPressao() const;
    double getLimiteBaixo() const;
    double getLimiteAlto() const;
    double getLimiteCritico() const;
    double getLimitePressao() const;

    SensorTemperatura& getSensorTemp();
    SensorNivel& getSensorNivel();
    SensorPressao& getSensorPress();
};