//TRABALHO DE PROGRAMAÇÃO ORIENTADA A OBJETOS
//Quando alguém quiser usar o líquido para um processo, e o tanque estiver no limite baixo, mesmo que a bomba de reabastecimento esteja aberta, precisa ter um alarme que dispara avisando ao usuário que ele não pode usar esse tanque até que ele atinja o nível baixo de segurança + nível que ele quer usar no processo
#include <iostream>
#include <string>
#include <random> // Biblioteca para gerar números aleatórios

using namespace std;
//ID_DUPLA = 160 ou 25

class Sensor{
    protected: // Assim a main não altera nada mas os sensores que herdarem a classe pode realizar as alterações necessárias.
    string tag;
    string unidade;
    double valor;

    public: 
    sensor(string t, string u) : tag(t), unidade(u), valor(0.0) {} // As chaves ficam vazias pois não tem nenhuma outra ação para realizar, já foi inicializado as variáveis

    virtual ~sensor(){} // Destruidor de objetos/classes - me parece EXTREMAMENTE desnecessário mas mantive para ver a diferença que faz no código

    virtual void simular() = 0; // Cada sensor simula de uma maneira diferente

    // 'const' pq são funções que servem apenas para leitura de dados, sem alterações
    // 'return tag' para retornar o nome de cada sensor
    string getTag() const {return tag;}
    string getUnidade() const {return unidade;}
    string getValor() const {return valor;}
    };

// SensorNivel herdando Sensor
class SensorNivel : public Sensor {
    public:
    // Chama o construtor e
    // 'Empurra' o texto "t" para a classe "mãe" e avisa que a unidade de medida utilizada nesta classe será %
    SensorNivel(string t) : sensor(t, "%") {
        valor = 70.0;
} 
    // Para a função simular vamos tentar usar o conceito chamado "Caminhada Aleatória (Random Walk)"
    // Em vez de sortear números completamente novos e aleatórios pegamos o 'valor' e sorteamos uma pequana variação (uma "pertubação") para subtrair dele
    void simular() override { 
        // 1. Configurar o gerador de números aleatórios do C++

        // 2. Definir que a variação vai ser pequena: entre [-X.X% e X.X%] 

        // 3. Aplicar a pequena variação no valor atual

        // 4. Segurança: não deixa o reservatório passsar do 100% nem ficar negativo [(< 0.0%)]
    }};

    // Essa aqui é a classe (Pai) - é um 'contrato' que as regras devem seguir 
    // Armazena a estrutura principal de todas as classes que forem expicificamente de alguma regra do sistema
class RegraControle{
    public:
    virtual void aplicar(EstacaoBombeamento& estacao) = 0; // '= 0' significa: função sem lógica aqui "{}", as classes filhas são OBRIGADAS a programar a lógica para cada classe especifica
    };

// Quando o nível da estação estiver abaixo de ['X']
class NivelBaixo : public RegraControle{
    public:
    void aplicar(EstacaoBombeamento& estacao) override {
        // sem '= 0' aqui, pois agora estamos escrevendo a lógica 
        // 'override' garante que estamos cumprindo o contrato do 'pai'
        if (estacao.nivel < [27.5]){ // % -> unidade de medida pedido na especificação do projeto
            estacao.ligarbomba;
}}};

//Quando a pressão da estação estiver maior ou igual a ['X']
class PressaoAlta : public RegraControle{
    public:
    void aplicar(EstacaoBombeamento& estacao) override{
        // sem '= 0' aqui, pois agora estamos escrevendo a lógica 
        // 'override' garante que estamos cumprindo o contrato do 'pai'
        if(estacao.pressao >= [6.5]) { // bar -> unidade de medida pedido na especificação do projeto
            estacao.desligarbomba;
}}};

// Quando o nível da estação estiver acima de ['X']
class NivelAlto : public RegraControle{
    public:
    void aplicar(EstacaoBombeamento& estacao) override{
        // sem '= 0' aqui, pois agora estamos escrevendo a lógica 
        // 'override' garante que estamos cumprindo o contrato do 'pai'
        if(estacao.nivelAlto > [82.5]){ // % -> unidade de medida pedido na especificação do projeto
            estacao.DesligarBomba; // Criar função DesligarBomba
}}};