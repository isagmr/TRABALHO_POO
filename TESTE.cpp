// TRABALHO DE PROGRAMAÇÃO ORIENTADA A OBJETOS
// Quando alguém quiser usar o líquido para um processo, e o tanque estiver no limite baixo, mesmo que a bomba de reabastecimento esteja aberta, precisa ter um alarme que dispara avisando ao usuário que ele não pode usar esse tanque até que ele atinja o nível baixo de segurança + nível que ele quer usar no processo
// Ver as uilizações da biblioteca stdexcept (erros)
// incluir sensor de temperatura
// Separar os componentes em vários arquivos e pensar nas estratéias (situações possíveis que o programa vai decidir sozinho qual usar)


#include <iostream>
#include <string>
#include <random> // Biblioteca para gerar números aleatórios (o Random Walk do código)
#include <thread> // Biblioteca que deixa pausar a execução do programa
#include <chrono> // Biblioteca para utilizar unidade de tempo

using namespace std;
//ID_DUPLA = 160 ou 25

class Sensor {
    protected: // Assim a main não altera nada mas os sensores que herdarem a classe pode realizar as alterações necessárias.
    string Tag;
    string Unidade;
    double Valor;

    public: 
    Sensor(string t, string u) : Tag(t), Unidade(u),Valor(0.0) {} // As chaves ficam vazias pois não tem nenhuma outra ação para realizar, já foi inicializado as variáveis

    virtual ~Sensor(){} // Destruidor de objetos/classes - me parece EXTREMAMENTE desnecessário mas mantive para ver a diferença que faz no código

    virtual void simular(bool BombaLigada, bool ValvulaAberta) = 0; // Cada sensor simula de uma maneira diferente

    // 'const' pq são funções que servem apenas para leitura de dados, sem alterações
    // 'return tag' para retornar o nome de cada sensor
    string getTag() const {return Tag;}
    string getUnidade() const {return Unidade;}
    double getValor() const {return Valor;}
    };

// SensorNivel herdando Sensor
class SensorNivel : public Sensor {
    public:
    // Chama o construtor e
    // 'Empurra' o texto "t" para a classe "mãe" e avisa que a unidade de medida utilizada nesta classe será %
    SensorNivel(string t) : Sensor(t, "%") {
        Valor = 90.0;
    } 
    // Para a função simular vamos tentar usar o conceito chamado "Caminhada Aleatória (Random Walk)"
    // Em vez de sortear números completamente novos e aleatórios pegamos o 'valor' e sorteamos uma pequana variação (um "ruido") para subtrair dele
    void simular(bool BombaLigada, bool ValvulaAberta) override { 
        if(BombaLigada == true){ // Se a bomba estiver ligada 
            static std::random_device rd;
	        static std::mt19937 gen(rd());
            // 2. Definir que a variação vai ser pequena: entre [0.5% e 1.5%] 
	        std::uniform_real_distribution<double> variacao(0.5, 1.5);
            // 3. Aplicar a pequena variação no valor atual
	        Valor += variacao(gen);
            // 4. Segurança: não deixa o reservatório passsar do 100% nem ficar negativo (< 0.0%)
	        if(Valor > 100.0){
		    Valor = 100.0;
	        }
	        if(Valor < 0.0){
		    Valor = 0.0;
	        }    
        }
        else if(ValvulaAberta == true){
            static std::random_device rd;
	        static std::mt19937 gen(rd());
            // 2. Definir que a variação vai ser pequena: entre [-0.5% e -1.5%] 
	        std::uniform_real_distribution<double> variacao(-0.5, -1.5);
            // 3. Aplicar a pequena variação no valor atual
	        Valor += variacao(gen);
            // 4. Segurança: não deixa o reservatório passsar do 100% nem ficar negativo (< 0.0%)
	        if(Valor > 100.0){
		    Valor = 100.0;
	        }
	        if(Valor < 0.0){
		    Valor = 0.0;
	        } 
        }
        else{
            // 1. Configurar o gerador de números aleatórios do C++
	        static std::random_device rd;
	        static std::mt19937 gen(rd());

            // 2. Definir que a variação vai ser pequena: entre [-1.5% e 1.5%] 
	        std::uniform_real_distribution<double> variacao(-1.5, 1.5);

            // 3. Aplicar a pequena variação no valor atual
	        Valor += variacao(gen);

            // 4. Segurança: não deixa o reservatório passsar do 100% nem ficar negativo (< 0.0%)
	        if(Valor > 100.0){
		        Valor = 100.0;
	        }
	        if(Valor < 0.0){
		        Valor = 0.0;
	        }
        }
    }
};

class SensorPressao : public Sensor {
    public: 
    SensorPressao(string t) : Sensor(t, "bar"){
        Valor = 0.0;
    }
    void simular (bool BombaLigada, bool AbrirValvula) override {
        if(BombaLigada == true){   // if(BombaLigada)
            static std::random_device rd;
	        static std::mt19937 gen(rd());
            // 2. Definir que a variação vai ser pequena: entre [0.3% e 0.8%] 
	        std::uniform_real_distribution<double> variacao(0.3, 0.8);
            // 3. Aplicar a pequena variação no valor atual
	        Valor += variacao(gen);
            // 4. Segurança: não deixa a pressão passsar de 10
	        if(Valor > 10.0){
		    Valor = 10.0;
	        }
        }

	    if(BombaLigada == false){    // if(!BombaLigada)
            static std::random_device rd;
	        static std::mt19937 gen(rd());
            // 2. Definir que a variação vai ser pequena: entre [-0.3% e -0.8%] 
	        std::uniform_real_distribution<double> variacao(-0.3, -0.8);
            // 3. Aplicar a pequena variação no valor atual
	        Valor += variacao(gen);
            // 4. Segurança: não deixa a pressão ficar negativa (< 0.0%)
	        if(Valor < 0.0){
		    Valor = 0.0;
	        }    

        }
    }

};

class SensorTemperatura : public Sensor {
    public:
    SensorTemperatura(string T) : Sensor(T, "°C") {
       Valor = 23.0;
   }
   
   void simular(bool BombaLigada, bool ValvulaAberta) override{
   
   }
};

class EstacaoBombeamento{
   private:
   double Nivel;
   bool BombaEntradaLigada;
   bool ValvulaAberta;
   double Pressao;
   double Temperatura; 

   const double LIMITE_BAIXO = 27.5;
   const double LIMITE_ALTO = 82.5;
   const double PRESSAO_ALTA = 6.5;
   const double TEMPERATURA_ALTA = 73.0;
   const double TEMPERAURA_BAIXO = 15.0;
 
   public: 
   // Construtor: Inicializa o sistema com valores padrão quando ele é criado
    EstacaoBombeamento() {
        Nivel = 70.0;                          // Começa em um nível seguro
        BombaEntradaLigada = false;            // Começa desligada
        Pressao = 00.0;                        // Começa em zero
        ValvulaAberta = false;                 // Começa desligada
        Temperatura = 23.0;                    // Começa em uma temperaura ok
   }    
   
   // Os getters pros sensores e regras lerem os estados
    double getNivel(){
        return Nivel;
    }
   
    double getNivelBaixo(){
        return LIMITE_BAIXO;    
    }

    bool isBombaEntradaLigada(){
        return BombaEntradaLigada;
   }
   
    bool isValvulaAberta(){
        return ValvulaAberta;
   }
   
    double getPressaoAlta() {
        return Pressao;
   }

   double getTemperatura(){
    return Temperatura;
   }
   
   // Os setters para as regras de controle
    void setNivel(double N){
        Nivel = N;
   }

    void setPressao(double P){
        Pressao = P;
   }

    void setTemperatura(double T){
        Temperatura = T;
   }

    void LigarBombaEntrada() {
        BombaEntradaLigada = true;
   }
   
    void DesligarBombaEntrada() {
        BombaEntradaLigada = false;
   }
   
    void AbrirValvula(){
        ValvulaAberta = true;
   }
   
    void FecharValvula() {
        ValvulaAberta = false;
   }

   // Usando o STDEXCEPT
    void UsarLiquidoDoProcesso(double NivelDesejado){
        if(Nivel <= LIMITE_BAIXO){
            double NivelNecessario = LIMITE_BAIXO + NivelDesejado;
            // Dispara o alarme
            throw std::runtime_error("BLOQUADO! Tanque no limite baixo de segurança. Aguarde atingir " + to_string(NivelNecessario) + "%");
        }
        
        Nivel -= NivelDesejado;
        if(Nivel < 0){
            Nivel = 0;
        }
   }
};

// Essa aqui é a classe Pai - 'contrato' que as regras devem seguir 
// Armazena a estrutura principal de todas as classes que forem expecificamente de alguma regra do sistema
class RegraControle{
    public:
    virtual void aplicar(EstacaoBombeamento& estacao) = 0; // '= 0' significa: função sem lógica aqui "{}", as classes filhas são OBRIGADAS a programar a lógica para cada classe especifica
};

// Quando o nível da estação estiver abaixo de 27.5
class NivelBaixo : public RegraControle{
    public:
    void aplicar(EstacaoBombeamento& estacao) override {
        // sem '= 0' aqui, pois agora estamos escrevendo a lógica 
        // 'override' garante que estamos cumprindo o contrato do 'pai'
        if (estacao.getNivel() < 27.5){ // % -> unidade de medida pedido na especificação do projeto
            estacao.LigarBombaEntrada();
            estacao.FecharValvula();
        }
    }
};

//Quando a pressão da estação estiver maior ou igual a 6.5
class PressaoAlta : public RegraControle{
    public:
    void aplicar(EstacaoBombeamento& estacao) override{
        // sem '= 0' aqui, pois agora estamos escrevendo a lógica 
        // 'override' garante que estamos cumprindo o contrato do 'pai'
        if(estacao.getPressaoAlta() >= 6.5) { // bar -> unidade de medida pedido na especificação do projeto
            estacao.DesligarBombaEntrada();
        }
    }
};

// Quando o nível da estação estiver acima de 82.5
class NivelAlto : public RegraControle{
    public:
    void aplicar(EstacaoBombeamento& estacao) override{
        // sem '= 0' aqui, pois agora estamos escrevendo a lógica 
        // 'override' garante que estamos cumprindo o contrato do 'pai'
        if(estacao.getNivel() > 82.5){ // % -> unidade de medida pedido na especificação do projeto
            estacao.DesligarBombaEntrada(); // Criar função DesligarBomba
	        estacao.AbrirValvula(); // Criar função AbrirValvula
        }
	if(estacao.getNivel() == 82.5){
            estacao.DesligarBombaEntrada();
           }
    }
};

int main (){
    EstacaoBombeamento estacao; // Estação Central
    
    // Criando os sensores e passando as tags
    SensorNivel sensorNivel("LT-61");
    SensorPressao sensorPressao("PT-61");
    SensorTemperatura sensorTemperatura("TT-61");
    
    // Criando as regras de controle
    NivelBaixo regraNivelBaixo;
    NivelAlto regraNivelAlto;
    PressaoAlta regraPressaoAlta;
    
    cout << "___ SIMULAÇÃO INICIADA ___" << endl;
    cout << "___ PRESSIONE [CNTRL + C] NO TERMINAL PARA ENCERRAR" << endl;
    
    int ciclo = 1;
    
    while(true){
        cout << "\n >>>> CICLO DE VARREDURA: " << ciclo << endl; // para acompanhar o ciclo de leitura em que estamos
    
        // 1: Os sensores simulam o ambiente físico baseado nos atuadores
        sensorNivel.simular(estacao.isBombaEntradaLigada(), estacao.isValvulaAberta());
        sensorPressao.simular(estacao.isBombaEntradaLigada(), estacao.isValvulaAberta());
        sensorTemperatura.simular(estacao.isBombaEntradaLigada(), estacao.isValvulaAberta());
    
        // 2: Injeção das leituras dos sensores para dentro do estado da estação
        estacao.setNivel(sensorNivel.getValor());
        estacao.setPressao(sensorPressao.getValor());
        estacao.setTemperatura(sensorTemperatura.getValor());
    
        // 3: Tela do Supervisor - pra mostrar o relatório em tempo real
        cout << " [ATUADORES] | BOMBA DE ENTRADA: " << (estacao.isBombaEntradaLigada() ? "Ligada" : "Desligada") << " | VÁLVULA DE SAÍDA: " << (estacao.isValvulaAberta() ? "Aberta" : "Fechada") << endl;
    
        cout << " [LEITURAS] " 
        << sensorNivel.getTag() << ":" << sensorNivel.getValor() << " " << sensorNivel.getUnidade() << " | "
        << sensorPressao.getTag() << ":" << sensorPressao.getValor() << " " << sensorNivel.getUnidade() << " | "
        << sensorTemperatura.getTag() << ":" << sensorTemperatura.getValor() << " " << sensorTemperatura.getUnidade() << " | " << endl;
    
        // 4: vai processar as regras de controle e tomar decisões para o próximo ciclo
        regraNivelBaixo.aplicar(estacao);
        regraNivelAlto.aplicar(estacao);
        regraPressaoAlta.aplicar(estacao);
    
        // 5: Pausa de 8 segundos antes de realizar a próxima checagem
        std::this_thread::sleep_for(std::chrono::seconds(8));
    
        // Incrmenta o valor do ciclo,
        ciclo ++;
    }
    
    return 0; // Na teoria o programa nunca vai chegar aqui
}