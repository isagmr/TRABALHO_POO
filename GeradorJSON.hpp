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

class GeradorJSON {
    private:
    ofstream Arquivo;
    string gerarTimestamp();

    public:
    GeradorJSON();
    ~GeradorJSON();
    void escreverLeitura(string tag, string variavel, double valor, string unidade, string status, int ciclo); //escreve uma leitura do sensor no arquivo
    void escreverAtuador(string nome, string estado, double valor, int ciclo); // Escreve o estado de um atuador (bomba ou válvula)
    void escreverAlarme(string codigo, string severidade, int ciclo); //escreve um alarme ativo
    void escreverComando(string nomeComando, string usuario, int ciclo); //escreve um comando executado pelo operador

};