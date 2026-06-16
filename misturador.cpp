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
#include <vector> // para listas de usuários e agendamentos de manutenções
#include <stdexcept> // para lançar exceções de acesso negado
#include <fstream> // salvar histórico em arquivo

using namespace std;
//ID_DUPLA = 160 ou 25

class RegistroSistema {
    private:
    ofstream ArquivoAlarmes;
    ofstream ArquivoAcoes;
    int CicloAtual;

    public:
    RegistroSistema() {
        CicloAtual = 0;

        ArquivoAlarmes.open("historico_alarmes.txt", ios::app); //ios::app significa que novas entradas são adicionadas ao fim do arquivo sem apagar oq já estava salvo
        ArquivoAcoes.open("historico_acoes.txt", ios::app);

        //para marcar uma nova sessão nos arquivos sempre que houver
        ArquivoAlarmes << "\n=== NOVA SESSAO ===" << endl;
        ArquivoAcoes << "\n=== NOVA SESSAO ===" << endl;
    }

    //destrutor da classe, garante que os arquivos serão fechados corretamente (segurança - não corromper)
    ~RegistroSistema() {
        ArquivoAlarmes.close();
        ArquivoAcoes.close();
    }

    //variável recebe o numero do ciclo e está protegida por encapsulamento de modo que não da pra alterar ela pela main, ou seja
    //o valor do ciclo segue o fluxo do processo
    void setCiclo(int C) {
        CicloAtual = C; 
    }

    //registra alarme no arquivo e exibe no terminal
    //usando concatenação de strings (somando textos); o + faz uma 'fusão' de textos; a função to_string transforma o numero do ciclo em texto
    void registrarAlarme(string Descricao){ //a descrição já é uma string q veio do parametro da função
        string linha = "[CICLO " + to_string(CicloAtual) + "] ALARME: " + Descricao;
        cout << linha << endl; //envia pro terminal
        ArquivoAlarmes << linha << endl; //envia pro arquivo
    }

    //registra um alarme rconhecido: quem reconheceu fica salvo
    void registrarReconhecimentoAlarme(string Alarme, string NomeUsuario) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] ALARME RECONHECIDO: " + Alarme + " | Por: " + NomeUsuario;
        cout << linha << endl;
        ArquivoAlarmes << linha << endl;
    }

    //registra qualquer ação do operador
    void registrarAcaoOperador(string NomeUsuario, string Acao) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] OPERADOR " + NomeUsuario + ": " + Acao;
        cout << linha << endl;
        ArquivoAcoes << linha << endl;
    }

    //registra as propostas de manutenções feitas pelo técnico
    void registrarPropostaManutencao(string NomeTecnico, string Equipamento, string Motivo, string Data) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] PROPOSTA DE MANUTENÇÃO" + " | Técnico: " + NomeTecnico + " | Equipamento " + Equipamento + " | Motivo: " + Motivo + " | Data: " + Data + " | Status: AGUARDANDO APROVAÇÃO";
        cout << linha << endl;
        ArquivoAcoes << linha << endl;
    }

    //registra aprovação de manutenção pelo admin
    void registrarRecusaManutencao(string NomeAdmin, string NomeTecnico, string Equipamento) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] MANUTENÇÃO APROVADA" + " | Admin: " + NomeAdmin + " | Técnico: " + NomeTecnico + " | Equipamento " + Equipamento;
        cout << linha << endl;
        ArquivoAcoes << linha << endl;
    }

    //registra recusa de manutenção pelo admin com motivo obrigatório
    void registrarRecusaManutencao(string NomeAdmin, string NomeTecnico, string Equipamento, string MotivoRecusa) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] MANUTENÇÃO RECUSADA" + " | Admin: " + NomeAdmin + " | Técnico: " + NomeTecnico + " | Equipamento " + Equipamento + " | Motivo: " + MotivoRecusa;
        cout << linha << endl;
        ArquivoAcoes << linha << endl;
    }

    //registra conclusão de manutenção
    void registrarConclusaoManutencao(string NomeTecnico, string Equipamento) {
        string linha = "[CICLO " + to_string(CicloAtual) + "] MANUTENÇÃO CONCLUÍDA" + " | Técnico: " + NomeTecnico + " | Equipamento " + Equipamento;
        cout << linha << endl;
        ArquivoAcoes << linha << endl;
    }
};

//definindo os níveis de acesso, onde quanto maior o número, mais permissões tem o usuário
enum class NivelAcesso {
    OPERADOR = 1; //ajusta os setpoints, vê leituras, faz pedidos de processo e reconhece alarmes
    TECNICO = 2; //agenda manutenção, coloca equipamentos em manutenção, acessa históricos
    ADMIN = 3; //desliga tudo, cadastra usuários, altera os limites de segurança do sistema, faz tudo dos outros dois em caso de emergência, permite manutenções, emite relatórios e registros
};

class Usuario{
    private:
    string Nome;
    string Senha;
    NivelAcesso Nivel;

    public:
    Usuario(string N, string S, NivelAcesso Nv) {
        Nome = N;
        Senha = S;
        Nivel = Nv;
    }

    bool verificarSenha(string S) const {
        return Senha == S;
    }

    //transforma temporariamente as variaveis no tipo inteiro; a lógica é: cada nivel de acesso é representado por um numero, se o usuario logado for um
    //tecnico (2) e ele tentar apertar um botao que exige ser admin(3), a função faz a conversão e testa se 2 é >= 3, como não é, ela retorna
    //falso e barra o acesso do técnico
    bool temPermissao(NivelAcesso NivelNecessario) const {
        return static_cast<int>(Nivel) >= static_cast<int>(NivelNecessario);
    }

    string getNome() const { return Nome; }
    NivelAcesso getNivel() const { return Nivel; }

    //'traduz' o nivel numerico para texto
    string getNivelTexto() const {
        if (Nivel == NivelAcesso::ADMIN) return "Administrador";
        if (Nivel == NivelAcesso::TECNICO) return "Técnico";
        return "Operador";
    }
};

//Gerenciador de login do sistema
class SistemaAcesso {
    private:
    vector<Usuario> Usuarios; //é como uma lista dinâmica (um array que pode crescer) que armazena os usuarios cadastrados
    Usuario* UsuarioAtivo; //é um ponteiro que aponta diretamente pro usuario que acabou de fazer login. se ninguem estiver logado, ele aponta pro vazio 
    RegistroSistema& Registro; //é uma referência; em vez de criar um sistema de registro novo, ele conecta diretamnete ao historico central criado antes, é assim q o sistema de login vai gravar coisas no arquivo de texto

    public:
    SistemaAcesso(RegistroSistema& R) : Registro(R) { //: Registro(R) é uma lista de inicializalçao; como Registro é uma referência e uma referência não pode
        //nascer vazia, então ela se conecta ao objeto Regitro que nasceu na main e o R é só um apelido que transporta o objeto da main pra dentro da classe
        UsuarioAtivo = nullptr; //começa apontando pro nada
        Usuarios.push_back(Usuario("admin", "admin123", NivelAcesso::ADMIN)); //push_back é adicionar ao final da lista
        Usuarios.push_back(Usuario("tecnico", "tec456", NivelAcesso::TECNICO));
        Usuarios.push_back(Usuario("operador", "op789", NivelAcesso::OPERADOR));
    }

    bool login(string Nome, string Senha) {
        for (int i = 0; i < (int)Usuarios.size(); i++) { //Usuarios.size() devolve quantos usuarios tem casdastrados (tamanho do vetor); esse loop testa usuário por usuário pra validar as credenciais
            if (Usuarios[i].getNome() == Nome && Usuarios[i].verificarSenha(Senha)) { //checa se o usario e a senha digitados são identicos aos cadastrados
                UsuarioAtivo = &Usuarios[i]; //guarda o endereço desse usuário especifico que acabou de logar no ponteiro UsuarioAtivo PRA SABERMOS QUEM TÁ OPERANDO o sistema a partir desse momento
                cout << "[ACESSO] Bem-vindo, " << Nome << " (" << Usuarios[i].getNivelTexto() << ")" << endl; //imprime as boas vindas junto com o nome de usuario e o cargo
                //usar ponteiro nesse caso da assistencia pra modificar as informações de uma vez em vez de criarmos uma cópia de informações e no caso de alterações
                // o computador segue o endereço ("vai até a casa dele") e altera a senha diretamente no usuário real que está dentro da lista
                return true;
            }
        }
        cout << "[ACESSO] Usuário ou senha incorretos." << endl;
        return false;
    }

    void logout() {
        if (UsuarioAtivo != nullptr) {
            cout << "[ACESSO] Logout: " << UsuarioAtivo->getNome() << endl; //como UsuarioAtivo é um ponteiro, não da pra usar . pra chamar, por isso o uso da seta ->
            //ele vai até o endereço e chama a função getNome que tem lá dentro
        }
        UsuarioAtivo = nullptr; //pra deslogar o usuário o ponteiro volta a apontra pro vazio
    }

    bool estaLogado() const {
        return UsuarioAtivo != nullptr; //se o ponteiro for diferente de vazio, tem um endereço guaraddomali, então alguem está logado
    }

    //função que valida o acesso
    void exigirPermissao(NivelAcesso NivelNecessario) const {
        if (UsuarioAtivo == nullptr) {
            //throw runtime_error é uma clausula de guarda que joga um erro na tela e para o processo;vem da biblioteca stdexcept; throw é um try-catch
            throw runtime_error("ACESSO NEGADO: Nenhum usuário logado.");
        }
        if (!UsuarioAtivo->temPermissao(NivelNecessario)) {
            throw runtime_error("ACESSO NEGADO: " + UsuarioAtivo->getNome() + " não tem permissão para esta operação.");
        }
    }

    //acessa quem está logado no momento
    string getNomeAtivo() const {
        if (UsuarioAtivo == nullptr) return "Ninguém"; //devolve 'ninguem' caso n tenha ninguem logado
        return UsuarioAtivo->getNome();
    }

    //se o ponteiro for inválido (ninguem logado) a resposta padrão é operador
    NivelAcesso getNivelAtivo() const {
        return UsuarioAtivo ? UsuarioAtivo->getNivel() : NivelAcesso::OPERADOR;
    }
};

class Bomba {
    private:
    string Tag;
    bool Ligada;
    bool Falha;
    double Potencia; //varia de  a 100%
    double CapacidadeMaxima; //litros por minuto a 100% de potência e válvula 100% aberta

    public:
    Bomba(string T, double CapMax){
        Tag = T;
        Ligada = false;
        Falha = false;
        Potencia = 0.0;
        CapacidadeMaxima = CapMax;
    }

    void Ligar(double PotenciaDesejada){
        if (Falha == true){
            return; // com defeito não liga
        }
        Ligada = true;
        Potencia = PotenciaDesejada;
        if (Potencia > 100.0) Potencia = 100.0;
        if (Potencia < 0.0)   Potencia = 0.0;
    }

    void AjustarPotencia(double NovaPotencia){
        if (Falha == true || Ligada == false){
            return; // só ajusta se estiver ligada e sem falha
        }
        Potencia = NovaPotencia;
        if (Potencia > 100.0) Potencia = 100.0;
        if (Potencia < 0.0)   Potencia = 0.0;
    }

    void Desligar(){
        Ligada = false;
        Potencia = 0.0;
    }

    void AtivarFalha(){
        Falha = true;
        Ligada = false;
        Potencia = 0.0;
    }

    //aqui retornamos a vazão real que a bomba está produzindo, e pra calcular completamente, a válvula precisa passar sua abertura
    double calcularVazao(double AberturaValvula) const {
        if (Ligada == false || Falha == true){
            return 0.0;
        }
        return (Potencia / 100.0) * (AberturaValvula / 100.0) * CapacidadeMaxima;
    }

    bool estaLigada() const { return Ligada; }
    bool temFalha() const { return Falha; }
    double getPotencia() const { return Potencia; }
    double getCapacidadeMaxima() const { return CapacidadeMaxima; }
    string getTag() const { return Tag; }

};

//o sistema vai usar isso para decidir automaticamente os parâmetros sem a intervenção do operador
class CalculadoraDemanda{
    public:

    //guarda o resultado do cálculo - tudo varia de 0 a 100%
    struct Parametros {
        double AberturaValvulaQuente;
        double AberturaValvulaFria;
        double PotenciaBombaQuente;
        double PotenciaBombaFria;
    };

    //recebe o que o operador quer e devolve o que o sistema deve fazer; temDesejada é a temperatura em °C que o operador pediu e volumePorMinuto é a vazão de saída desejada em l/min
    static Parametros calcular(double TempDesejada, double VolumePorMinuto) {
        Parametros p;

        const double TEMP_QUENTE = 70.0;
        const double TEMP_FRIA  = 20.0;

        //garantindo que a temperatura desejada está dentro dos limites possíveis
        if (TempDesejada >= TEMP_QUENTE) TempDesejada = TEMP_QUENTE - 1.0;
        if (TempDesejada <= TEMP_FRIA) TempDesejada = TEMP_FRIA + 1.0;

        //calculando a fração de cada fonte necessária p/ atingir a temperatura desejada
        double fracaoQuente = (TempDesejada - TEMP_FRIA) / (TEMP_QUENTE - TEMP_FRIA);
        double fracaoFria = 1.0 - fracaoQuente;

        //as válvulas abrem proporcionalmente à fração necessária garantindo que a mistura saia na temperatura certa
        p.AberturaValvulaQuente = fracaoQuente * 100.0;
        p.AberturaValvulaFria = fracaoFria * 100.0;

        //visando a durabilidade, as bombas operam a no máx 70% da sua potência em condições normais, p/ preservar a vida útil e evitar superaquecimento
        //o volume pedido pelo operador é atingido pela abertura das válvulas, sem forcar as bombas
        double potenciaBase = 70.0;

        //se o volume pedido for baixo, a potência é ainda mais reduzida. temos como referência que 100 l/min é volume alto no sistema
        double fatorVolume = VolumePorMinuto / 100.0;
        if (fatorVolume > 1.0) fatorVolume = 1.0;

        p.PotenciaBombaQuente = potenciaBase * fatorVolume;
        p.PotenciaBombaFria = potenciaBase * fatorVolume;

        //garantindo potência mínima em 20% pra manter o fluxo estável
        if (p.PotenciaBombaQuente < 20.0) p.PotenciaBombaQuente = 20.0;
        if (p.PotenciaBombaFria < 20.0) p.PotenciaBombaFria = 20.0;

        return p;
     }
};


class Valvula{ // Precisa ter abertura de 0 a 100%
    private:
    string Tag;
    double Abertura; // 0-100%
    bool Travada;
    
    public:
    Valvula(string T){
        Tag = T;
        Abertura = 0.0;
        Travada = false;
    }

    void Abrir(double Percentual){
        if(Travada == true){
            return;   // Se a válvula estiver travada não muda nada
        }
        
        Abertura = Percentual;
        
        if(Abertura > 100.0){
            Abertura = 100.0;
        }
        if (Abertura < 0.0){
            Abertura = 0.0;
        }
    }

    void Fechar(){
        if (Travada == false){
            Abertura = 0.0;
        }
    }

    double getAbertura() const {
        return Abertura;
    }
};


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

        // 1. Configurar o gerador de números aleatórios do C++
        static std::random_device rd;
	    static std::mt19937 gen(rd());

        if(BombaLigada == true){ // Se a bomba estiver ligada 
            
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
            
            // 2. Definir que a variação vai ser pequena: entre [-0.5% e -1.5%] 
	        std::uniform_real_distribution<double> variacao(-1.5, -0.5);
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
    SensorPressao(string t) : Sensor(t, "BAR"){
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

	    else if(BombaLigada == false){    // if(!BombaLigada)
            static std::random_device rd;
	        static std::mt19937 gen(rd());
            // 2. Definir que a variação vai ser pequena: entre [-0.3% e -0.8%] 
	        std::uniform_real_distribution<double> variacao(-0.8, -0.3);
            // 3. Aplicar a pequena variação no valor atual
	        Valor += variacao(gen);
            // 4. Segurança: não deixa a pressão ficar negativa (< 0.0%)
	        if(Valor < 0.0){
		    Valor = 0.0;
	        }    

        }
    }

};

class SensorTemperatura : public Sensor { // Calcular temperatura com base nas vazoes quente/fria.
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
        BombaEntradaLigada = false;            // Começa desligada       / BombaQuente e BombaFria como objetos.
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
   
    bool isValvulaAberta(){  //ValvulaQuente e ValvulaFria com abertura 0-100%.
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
    virtual ~RegraControle() = default;
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

    //TIRAR DEPOIS
    CalculadoraDemanda calc;
    CalculadoraDemanda::Parametros p = CalculadoraDemanda::calcular(45.0, 60.0);

    cout << "--- CALCULO AUTOMATICO ---" << endl;
    cout << "Valvula quente: " << p.AberturaValvulaQuente << "%" << endl;
    cout << "Valvula fria:   " << p.AberturaValvulaFria   << "%" << endl;
    cout << "Potencia bomba quente: " << p.PotenciaBombaQuente << "%" << endl;
    cout << "Potencia bomba fria:   " << p.PotenciaBombaFria   << "%" << endl;
    
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
        << sensorPressao.getTag() << ":" << sensorPressao.getValor() << " " << sensorPressao.getUnidade() << " | "
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