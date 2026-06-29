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

#include "PlantaMistura.hpp"
using namespace std;

class Comando { //aqui criamos um molde abstrato que todo e qualquer comando do sistema  deve seguir, garantindo que todos os comandos terão a função executar() e nome()
    public:
    virtual ~Comando() = default; //garante que quando um comando for deletado da memória atraves de um ponteiro da classe mae, a memoria da classe filha seja limpa corretamente, evitando vazamentos de memória
    virtual void executar(PlantaMistura& planta) = 0; //contrato obrigatório
    virtual string nome() const = 0; //identifica o comando no histórico
};