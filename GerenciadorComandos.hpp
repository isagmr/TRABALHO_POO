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

class GerenciadorComandos {
// A seção 'private' garante o Encapsulamento. Nenhuma outra parte do código
// consegue alterar essas variáveis diretamente, apenas através dos métodos da classe.
private:
    double setpoint_temp;      // Guarda a temperatura alvo desejada
    double vazao_alvo;         // Guarda a vazão desejada do sistema
    bool override_bomba_q;     // True se o admin ligar a bomba quente manualmente
    bool override_bomba_f;     // True se o admin ligar a bomba fria manualmente
    bool parada_emergencia;    // True se o botão de emergência for acionado
    std::string nome_arquivo;  // Nome do arquivo que será lido (comandos.json)

    void limparTexto(std::string &valor);

public:
    GerenciadorComandos(std::string arquivo = "comandos.json");
    void atualizarComandos();
    double getSetpointTemp() const;    
    double getVazaoAlvo() const;       
    bool getOverrideBombaQ() const;    
    bool getOverrideBombaF() const;   
    bool getParadaEmergencia() const;  
};