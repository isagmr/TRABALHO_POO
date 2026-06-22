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
    void registrarAprovacaoManutencao(string NomeAdmin, string NomeTecnico, string Equipamento) {
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
    OPERADOR = 1, //ajusta os setpoints, vê leituras, faz pedidos de processo e reconhece alarmes
    TECNICO = 2, //agenda manutenção, coloca equipamentos em manutenção, acessa históricos
    ADMIN = 3 //desliga tudo, cadastra usuários, altera os limites de segurança do sistema, faz tudo dos outros dois em caso de emergência, permite manutenções, emite relatórios e registros
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

enum class StatusManutencao {
    NORMAL,
    ATENCAO,
    MANUTENCAO
};

struct AgendamentoManutencao {
    string NomeEquipamento;
    string Motivo;
    string DataAgendada;
    string NomeTecnico;
    string NomeAdmin;
    string MotivoRecusa;
    bool Aprovada;
    bool Pendente;
    bool Concluida;
};

//acompanha o desgaste das bombas e válvulas pra avisar quando precisam de revisão, antes que quebrem
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
    MonitorEquipamento(string Nome) { //construtor - inicia tudo
        NomeEquipamento = Nome;
        HorasOperacao = 0.0;
        HorasEmPotenciaAlta = 0.0;
        TemperaturaOperacao = 25.0;
        CiclosLigaDesliga = 0;
        Status = StatusManutencao::NORMAL;
    }

    void atualizar(bool Ligada, double Potencia, double HorasCiclo) { //calcula o desgaste a cada ciclo; recebe 3 informações: a peça ta ligada? qual a potencia? quanto tempo passou?
        if (!Ligada) {
            if (TemperaturaOperacao > 25.0) TemperaturaOperacao -= 0.5; //se ta desligado e a peça ta quente, esfria um pouco
            return;
        }

        HorasOperacao += HorasCiclo; // soma o tempo que passou no total de horas trabalhadas

        if (Potencia > 80.0) { //desgaste por potencia
            HorasEmPotenciaAlta += HorasCiclo;
            TemperaturaOperacao += (Potencia/100.0)*0.3; //se a potencia for alta, o motor ta sofrendo; soma tempo na variavel e a temperatura sobe mais rapido
        } else {
            TemperaturaOperacao += (Potencia/100.0)*0.1; //se n for potencia alta, atemperatura sobe mais devagar
            if (TemperaturaOperacao > 60.0) TemperaturaOperacao -= 0.2; // se passar de 60 graus a refrigeração natural abaixa um pouco a temperatura
        }

        if (TemperaturaOperacao > 95.0) TemperaturaOperacao = 95.0; //impede que a temperatura passe de 95 na simulação

        verificarLimites(); //verifica se alguma soma estourou os limites de alerta
    }    

    void registrarCicloLigaDesliga() { //toda vez q a bomba for ligada ele soma 1 nos ciclos e checa os limites
        CiclosLigaDesliga++;
        verificarLimites();
    }

    void entrarEmManutencao() { //se o tecnico chamar essa função, o status muda pra manutenção e o equipamento para
        Status = StatusManutencao::MANUTENCAO;
    }

    void concluirManutencao() { //o tecnico terminou o serviço; o status volta pra NORMAL e as horas de desgaste pesado e os ciclos zeram e a temperatura volta a 25 graus, como se tivessem trocado o óleo e limpado o motor
        //as horas totais não zeram pq a peça continua sendo a mesma
        Status = StatusManutencao::NORMAL;
        HorasEmPotenciaAlta = 0.0;
        CiclosLigaDesliga = 0;
        TemperaturaOperacao = 25.0;
    }

    StatusManutencao getStatus() const { return Status; }
    double getHorasOperacao() const { return HorasOperacao; }
    double getHorasEmPotenciaAlta() const { return HorasEmPotenciaAlta; }
    double getTemperaturaOperacao() const { return TemperaturaOperacao; }
    int getCiclosLigaDesliga() const { return CiclosLigaDesliga; }
    string getNomeEquipamento() const { return NomeEquipamento; }

    string getStatusTexto() const {
        if (Status == StatusManutencao::MANUTENCAO) return "EM MANUTENÇÃO";
        if (Status == StatusManutencao::ATENCAO) return "ATENÇÃO";
        return "NORMAL";
    }

    string getMotivoAlerta() const {
        if (HorasOperacao >= LIMITE_HORAS_TOTAL) return "Limite de horas de operação atingido";
        if (HorasEmPotenciaAlta >= LIMITE_HORAS_FORCA) return "Excesso de horas em alta potência";
        if (TemperaturaOperacao >= LIMITE_TEMPERATURA) return "Temperatura de operação elevada";
        if (CiclosLigaDesliga >= LIMITE_CICLOS) return "Excesso de ciclos liga/desliga";
        return "";
    }

    private: // é privado pq só o próprio monitor usa
    void verificarLimites() {
        if (Status == StatusManutencao::MANUTENCAO) return;
        bool alertar = (HorasOperacao >= LIMITE_HORAS_TOTAL) || (HorasEmPotenciaAlta >= LIMITE_HORAS_FORCA) || (TemperaturaOperacao >= LIMITE_TEMPERATURA) || (CiclosLigaDesliga >= LIMITE_CICLOS);
        if (alertar) Status = StatusManutencao::ATENCAO;
    }

};

//CRIA um fluxo de trabalho: tecnico pede autorização, admin aprova ou recusa e depois vai pra manutenção
class GerenciadorManutencao {
    private:
    vector<AgendamentoManutencao> Agendamentos; //guarda os pedidos de manutenção
    vector<MonitorEquipamento*> Monitores; //pega o endereço das peças pra avisar q elas devem parar de rodar pra entrar em manutenção
    RegistroSistema& Registro; //salva no arquivo (não cria um novo, ele usa o que já foi aberto na main)

    public:
    GerenciadorManutencao(RegistroSistema& R) : Registro(R) {} //temos que passar o registro p ele nascer na main

    void registrarMonitor(MonitorEquipamento* M) { //adicionando as máquinas na lista do lista do gerenciador
        Monitores.push_back(M); //push back joga no final da lista
    }

    //Técnico propõe a manutenção e fica pendente ate o admin aprovar
    //pede o nome da maquina, o pq quebrou, a data e EXIGE o acesso pra ver quem ta logado
    void proporManutencao(string NomeEquip, string Motivo, string Data, SistemaAcesso& Acesso) {
        Acesso.exigirPermissao(NivelAcesso::TECNICO);

        AgendamentoManutencao ag; //criando uma 'prancheta de serviço' nova e em branco
        //preeche esses dados usando os parâmetros da função
        ag.NomeEquipamento = NomeEquip;
        ag.Motivo = Motivo;
        ag.DataAgendada = Data;
        ag.NomeTecnico = Acesso.getNomeAtivo();  //puxa automaticamente os dados de quem ta logado
        //preenche o resto com dados vazios / padrões pq ainda n houve aprovação/recusa
        ag.NomeAdmin = "";
        ag.MotivoRecusa = "";
        ag.Aprovada = false;
        ag.Pendente = true;
        ag.Concluida = false;
        Agendamentos.push_back(ag); //guradndo essa prancheta dentro da lista agendamentos

        Registro.registrarPropostaManutencao(Acesso.getNomeAtivo(), NomeEquip, Motivo, Data); //escrevendo no arquivo q o tecnico pediu essa mantenção
    }

    //admin aprovou uma proposta pendente
    void aprovarManutencao(string NomeEquip, SistemaAcesso& Acesso) {
        Acesso.exigirPermissao(NivelAcesso::ADMIN);

        for (auto& ag : Agendamentos) { //abre a lista agendamentos e olha prancheta por prancheta; auto& garante q é a prancheta original e não uma cópia; o ag é a prancheta usada no momneto
            if (ag.NomeEquipamento == NomeEquip && ag.Pendente) { //testa se o nome do equipamento é o mesmo q o admin quer aprovar e se está como pendente
                //se for a certa, ele altera o status e assina o nome do admin logado
                ag.Aprovada = true;
                ag.Pendente = false;
                ag.NomeAdmin = Acesso.getNomeAtivo();

                Registro.registrarAprovacaoManutencao(Acesso.getNomeAtivo(), ag.NomeTecnico, NomeEquip);

                //avisa o monitor do equipamento
                for (auto* m : Monitores) { //olha a lista de monitores e olha máquina por maquina (m)
                    //se achar a máquina com o nome certo, chama a função entrar em manutnção, que congela o desgaste e avisa ao sistema q ela parou
                    if (m->getNomeEquipamento() == NomeEquip) {
                        m->entrarEmManutencao();
                    }
                }
                return; //como ja achou e aprovou a manutenção, n precisa continuar olhando as pranchetas
            }
        }
        cout << "[MANUTENÇÃO] Nenhuma proposta pendente encontrada para: " << NomeEquip << endl;
    }

    //admin recusa uma proposta com motivo obrigatório
    void recusarManutencao(string NomeEquip, string MotivoRecusa, SistemaAcesso& Acesso) {
        Acesso.exigirPermissao(NivelAcesso::ADMIN);

        for (auto& ag : Agendamentos) {
            if (ag.NomeEquipamento == NomeEquip && ag.Pendente) {
                ag.Aprovada = false;
                ag.Pendente = false;
                ag.NomeAdmin = Acesso.getNomeAtivo();
                ag.MotivoRecusa = MotivoRecusa;

                Registro.registrarRecusaManutencao(Acesso.getNomeAtivo(), ag.NomeTecnico, NomeEquip, MotivoRecusa);
                return;
            }
        }
        cout << "[MANUTENÇÃO] Nenhuma proposta pendente encontrada para: " << NomeEquip << endl;
    }

    //tecnico conclui a manutenção após a aprovação
    void concluirManutencao(string NomeEquip, SistemaAcesso& Acesso) {
        Acesso.exigirPermissao(NivelAcesso::TECNICO);

        for (auto& ag : Agendamentos) {
            if (ag.NomeEquipamento == NomeEquip && ag.Aprovada && !ag.Concluida) {
                ag.Concluida = true;

                Registro.registrarConclusaoManutencao(Acesso.getNomeAtivo(), NomeEquip);

                for (auto* m : Monitores) {
                    if (m->getNomeEquipamento() == NomeEquip) {
                        m->concluirManutencao();
                    }
                }
                return;
            }
        }
        cout << "[MANUTENÇÃO] Nenhuma manutenção aprovada pendente para: " << NomeEquip << endl;
    }

    //verifica alarmes automáticos e registra no arquivo
    void verificarAlertas() {
        for (auto* m : Monitores) {
            if (m->getStatus() == StatusManutencao::ATENCAO) {
                Registro.registrarAlarme(m->getNomeEquipamento() + ": " + m->getMotivoAlerta() + " | Solicite manutenção ao técnico.");
            }
        }
    }

    //exibe só pro admin; essa função é proibida de acessar qualquer variavel, só pode ler, por isso o const
    void exibirHistorico(SistemaAcesso& Acesso) const {
        Acesso.exigirPermissao(NivelAcesso::ADMIN);
        cout << "\n-- HISTÓRICO DE MANUTENÇÕES --" << endl;

        if (Agendamentos.empty()) { cout << " Nenhum registro." << endl; return; } //testa se a lista ta completamente vazia

        for (const auto& ag : Agendamentos) {
            cout << " Equipamento: " << ag.NomeEquipamento << " | Tecnico: " << ag.NomeTecnico << " | Motivo: " << ag.Motivo << " | Data: " << ag.DataAgendada;
            if (ag.Pendente) {
                cout << " | AGUARDANDO APROVAÇÃO";
            } else if (ag.Aprovada) {
                cout << " | APROVADA por: " << ag.NomeAdmin;
                if (ag.Concluida) cout << " | CONCLUÍDA";
            } else {
                cout << " | RECUSADA por: " << ag.NomeAdmin << " | Motivo: " << ag.MotivoRecusa;
            }
            cout << endl;
        }
    }
};

class Bomba {
    private:
    string Tag;
    bool Ligada;
    bool Falha;
    double Potencia; //varia de  a 100%
    double CapacidadeMaxima; //litros por minuto a 100% de potência e válvula 100% aberta
    bool EstavaLigada; //detecta transição desligada -> ligada

    public:
    MonitorEquipamento Monitor;

    Bomba(string T, double CapMax) : Monitor(T) {
        Tag = T;
        Ligada = false;
        Falha = false;
        Potencia = 0.0;
        CapacidadeMaxima = CapMax;
        EstavaLigada = false;
    }

    void Ligar(double PotenciaDesejada){
        if (Falha == true){
            return; // com defeito não liga
        }
        if (Monitor.getStatus() == StatusManutencao::MANUTENCAO) {
            cout << "[AVISO] " << Tag << " está em manutenção." << endl;
            return;
        }
        if (!EstavaLigada) Monitor.registrarCicloLigaDesliga();
        Ligada = true;
        EstavaLigada = true;
        Potencia = PotenciaDesejada;
        if (Potencia > 100.0) Potencia = 100.0;
        if (Potencia < 0.0) Potencia = 0.0;
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
        EstavaLigada = false;
        Ligada = false;
        Potencia = 0.0;
    }

    void AtivarFalha(){
        Falha = true;
        Ligada = false;
        Potencia = 0.0;
        EstavaLigada = false;
    }

    void atualizarMonitor(double HorasCiclo) {
        Monitor.atualizar(Ligada, Potencia, HorasCiclo);
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

//redundância industrial: configuração standby ou failover, q tem 2 componentes de modo q quando um da problema, o outro assume
class ParBombas{
    private:
    Bomba Principal; //dois objetos bomba guardados dentro da classe
    Bomba Reserva;
    bool UsandoReserva;

    public:
    ParBombas(string TagPrincipal, string TagReserva, double CapMax) : Principal(TagPrincipal, CapMax), Reserva(TagReserva, CapMax) { //constrói as 2 bombas de uma vez só
        UsandoReserva = false; //por padrão, a reserva vem desativada
    }

    void Ligar(double Potencia) {
        if (deveUsarReserva()) { //verifica se deve usar a reserva
            if(!UsandoReserva) { //testa se ja tava usando a reserva
                cout << "[REDUNDÂNCIA] Alternando para reserva: " << Reserva.getTag() << endl;
                Principal.Desligar();
                UsandoReserva = true;
            }
            Reserva.Ligar(Potencia);
        }
    }

    void Desligar() { //funciona como um botão de emergencia; manda o sinal pras duas pararem
        Principal.Desligar();
        Reserva.Desligar();
    }

    double calcularVazao(double AberturaValvula) const {
        if (UsandoReserva) return Reserva.calcularVazao(AberturaValvula);
        return Principal.calcularVazao(AberturaValvula);
    }

    void AjustarPotencia(double Potencia) {
    if (UsandoReserva) Reserva.AjustarPotencia(Potencia);
    else  Principal.AjustarPotencia(Potencia);
}

    void atualizarMonitores(double HorasCiclo){ //a cada ciclo, avisa o tempo q passou pras duas bombas
        //mesmo a q esta desligada rece o aviso e usa esse tempo pra esfriar a temperatura
        Principal.atualizarMonitor(HorasCiclo);
        Reserva.atualizarMonitor(HorasCiclo);
    }

    //coloca a principal em manutenção e trasfere carga pra reserva
    void iniciarManutencaoPrincipal(SistemaAcesso& Acesso) {
        Acesso.exigirPermissao(NivelAcesso::TECNICO);
        cout << "[MANUTENÇÃO] Transferindo carga: " << Principal.getTag() << " -> " << Reserva.getTag() << endl;
        Principal.Monitor.entrarEmManutencao();
        Principal.Desligar();
        UsandoReserva = true;
    }

    //retornam uma referência ás bombas caso alguém de fora precise olhar os status delas
    Bomba& getPrincipal() { return Principal; }
    Bomba& getReserva() { return Reserva; }
    bool estaUsandoReserva() const { return UsandoReserva; }
    string getTagAtiva() const { return UsandoReserva ? Reserva.getTag() : Principal.getTag();} // essa sintaxe de ? é um operador ternário

    private:
    bool deveUsarReserva() const {
        return Principal.temFalha() || Principal.Monitor.getStatus() == StatusManutencao::MANUTENCAO;
    }
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
        if(Travada) return;   // Se a válvula estiver travada não muda nada
        
        Abertura = Percentual;
        
        if(Abertura > 100.0) Abertura = 100.0;
        if (Abertura < 0.0) Abertura = 0.0;
    }

    void Fechar(){
        if (!Travada) Abertura = 0.0;
    }

    void Travar() { Travada = true; }
    void Destravar() { Travada = false; }

    double getAbertura() const { return Abertura; }
    bool estaTravada() const { return Travada; }
    string getTag() const {return Tag; }
};

//serpentina -> equipamento que aquece ou resfria o líquido dos reservatórios
//ela é basicamente um cano em espiral que passa por dentro de um tanque. denttro dele pode passar água quente, vapor ou gás refrigerante, aí controla a temperatura
enum class TipoSerpentina { //criando uma lista de opções fixas; a serpentina no simulador só pode ser de dois tipos, evitando q alguem digite o tipo errado como texto
    AQUECIMENTO,
    RESFRIAMENTO
};

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

    Serpentina(string T, TipoSerpentina Tp, double TempAlvo) : Monitor(T) { //construtor; passamos a tag, o tipo e a temperatura alvo; ': Monitor(T)' cria o monitoramneto da saúde dela passando o nome automaticamente
       //configurando os valores iniciais
        Tag = T;
        Tipo = Tp;
        Potencia = 0.0;
        Ligada = false;
        Falha = false;
        TemperaturaAlvo = TempAlvo;
    }

    void Ligar(double PotenciaDesejada) {
        if (Falha) return; //maquina quebrada não liga!
        if (Monitor.getStatus() == StatusManutencao::MANUTENCAO) { //maquina em manutenção NÃO LIGA
            cout << "[AVISO] " << Tag << " esta em manutencao." << endl;
            return;
        }
        Ligada = true;
        Potencia = PotenciaDesejada;
        //se o operador digitar 150%, o sistema limita pra 100%, se digitar negativo, limita pra 0
        if (Potencia > 100.0) Potencia = 100.0;
        if (Potencia < 0.0) Potencia = 0.0;
    }

    void AjustarPotencia(double NovaPotencia) {
        if (Falha || !Ligada) return;
        //atualiza o valor e define os limites
        Potencia  = NovaPotencia;
        if (Potencia > 100.0) Potencia = 100.0;
        if (Potencia < 0.0) Potencia = 0.0;
    }

    void Desligar() {
        Ligada = false;
        Potencia = 0.0;
    }

    void AtivarFalha() { //força uma pane no equipamento; simula acidente ou quebra; desliga tudo e fica travada em estado de erro
        Falha = true;
        Ligada = false;
        Potencia = 0.0;
    }

    //calcula quanto a serpentina consegue mudar a temperatura do reservatório a cada ciclo baseado na sua potencia atual
    double calcularVariacaoTemperatura() const {
        if (!Ligada || Falha) return 0.0; //se estiver desligada ou quebrada, n esquenta nem esfria

        //100% de potencia causa variação máxima de 2 graus por ciclo; então a variação é proporcional a potencia atual
        double variacaoMaxima = 2.0;
        if (Tipo == TipoSerpentina::AQUECIMENTO) {
            return (Potencia / 100.0) * variacaoMaxima; //positico aquece
        } else {
            return -(Potencia /100.0) * variacaoMaxima; //negativo resfria
        }
    }

    void atualizarMonitor(double HorasCiclo) { //repassando os dadsos pro monitor calcular o desgaste termico a cada ciclo
        Monitor.atualizar(Ligada, Potencia, HorasCiclo);
    }

    //bloco de funlções get que só entrgam cópias dos valores privados, com o const garantindo que os dados não vão ser alterados
    bool estaLigada() const { return Ligada; }
    bool temFalha() const { return Falha; }
    double getPotencia() const { return Potencia; }
    double getTemperaturaAlvo() const { return TemperaturaAlvo; }
    string getTag() const { return Tag; }
    TipoSerpentina getTipo() const { return Tipo; }
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

        static std::random_device rd;
	    static std::mt19937 gen(rd());

        if(BombaLigada == true){   // if(BombaLigada)
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

//3 sensores de temperatura: reservatorio quente, frio e tanque de mistura
enum class TipoSensorTemp {
    RESERVATORIO_QUENTE, //le a temp do reservatorio quente
    RESERVATORIO_FRIO,
    TANQUE_MISTURA
};

class SensorTemperatura : public Sensor { //herda a classe mãe Sensor; ganha automaticamente todas as variáveis e funções que a classe mãe tiver
    private:
    TipoSensorTemp Tipo; //guarda o tipo do sensor baseado no enum

    public:
    SensorTemperatura(string T, TipoSensorTemp Tp, double ValorInicial) : Sensor(T, "°C") { //construtor
        Tipo = Tp;
        Valor = ValorInicial;
   }
   
   //simula leitura do reservatório quente ou frio com pequeno ruído; a serpentina vai manter o valor perto do alvo, então o ruído é pequeno
   void simular(bool BombaLigada, bool ValvulaAberta) override{ //override avisa ao compilador q estamos reescrevendo a função simular q já existia na classe mãe; recebe o estado das bombas e válvulas só pq a classe mãe exige, mas não usamos
        static random_device rd;
        static mt19937 gen(rd());

        //reservatorios tem o ruido pequeno pq a serpentina estabiliza
        if (Tipo == TipoSensorTemp::RESERVATORIO_QUENTE || Tipo == TipoSensorTemp::RESERVATORIO_FRIO) {
            uniform_real_distribution<double> ruido(-0.3, 0.3); //cria um sorteio de numeros quebrados entre -0,3 e 0,3 graus
            Valor += ruido(gen); //simula o ruido eletrico adicionando os valores sorteados antes; o sensor nunca vai medir a temperatura perfeitamente estatica, sempre vai ter uma pequena oscilação
        }
   }

   //o tanque de mistura é atualizado pelo atualizarValorMistura()
   void atualizarValorMistura(double NovaTemperatura) {
    Valor = NovaTemperatura;
   }

   //aplica o efeito da serpentina no reservatorio
   void atualizarValorReservatorio(double VariacaoSerpentina, double TempAlvo) {
    Valor += VariacaoSerpentina;
    //simula q o liquido resiste a mudanças bruscas de temperatura
    if (Tipo == TipoSensorTemp::RESERVATORIO_QUENTE) {
        if (Valor > 75.0) Valor = 75.0; //limite max de segurança
        if (Valor < 55.0) Valor = 55.0; //minimo: caldeira sempre ajuda
    } else if (Tipo == TipoSensorTemp::RESERVATORIO_FRIO) {
        if (Valor > 30.0) Valor = 30.0;
        if (Valor < 10.0) Valor = 10.0;
    }
   }

   TipoSensorTemp getTipo() const { return Tipo; } //entrega qual a utilidade e o tipo do sensor
};

//reservatórios: cada um mantem sua temperatura com a serpentina automaticamente
//cria o controle automático (tipo um termostato de ar-condicionado) dos tanques e calcula a física real de mistura de líquidos
class ReservatorioQuente {
    private:
    string Tag;
    Serpentina SerpentinaAquecimento;
    SensorTemperatura Sensor;

    const double TEMP_ALVO = 70.0;
    const double TOLERANCIA = 2.0; //aceita entre 68 e 72 graus

    public:
    ReservatorioQuente(string T) : Tag(T), SerpentinaAquecimento(T + "-SERP", TipoSerpentina::AQUECIMENTO, 70.0), Sensor(T + "-TT", TipoSensorTemp::RESERVATORIO_QUENTE, 70.0) { //adiciona o nome do sensor ao sufixo
        //liga a serpentina na potencia media ao iniciar
        SerpentinaAquecimento.Ligar(50.0);
    }

    //chamado a cada ciclo - controla a serpentina e atualiza o sensor
    void atualizar(double HorasCiclo) {
        double tempAtual = Sensor.getValor();

        //controle automático da serpentina
        if (tempAtual < TEMP_ALVO - TOLERANCIA) {
            //temperatura cauiu - aumenta a potencia da serpentina
            double novaPotencia = SerpentinaAquecimento.getPotencia() + 10.0;
            SerpentinaAquecimento.AjustarPotencia(novaPotencia);
        } else if (tempAtual > TEMP_ALVO + TOLERANCIA) {
            //temperatura subiu dms - reduz a potencia
            double novaPotencia = SerpentinaAquecimento.getPotencia() - 10.0;
            SerpentinaAquecimento.AjustarPotencia(novaPotencia);
        }

        //aplica o efeito da serpentina na temperatura do sensor
        double variacao = SerpentinaAquecimento.calcularVariacaoTemperatura();
        Sensor.atualizarValorReservatorio(variacao, TEMP_ALVO);
        Sensor.simular(false, false); //aplica o ruido natural

        //atualiza o monitor de desgaste da serpentina
        SerpentinaAquecimento.atualizarMonitor(HorasCiclo);
    }

    void simularFalhaSerpentina() { //teste pra quebrar a serpentina de propósito e ver se o sistema de alarmes funciona
        SerpentinaAquecimento.AtivarFalha();
        cout << "[FALHA] Serpentina do reservatorio quente falhou!" << endl;
    }

    double getTemperatura() const { return Sensor.getValor(); }
    string getTagSensor() const { return Sensor.getTag(); }
    Serpentina& getSerpentina() { return SerpentinaAquecimento; }
    SensorTemperatura& getSensor() { return Sensor; }
};

class ReservatorioFrio {
    private:
    string Tag;
    Serpentina SerpentinaResfriamento;
    SensorTemperatura Sensor;

    const double TEMP_ALVO = 20.0;
    const double TOLERANCIA = 2.0; //aceita entre 18 e 20 graus

    public:
    ReservatorioFrio(string T) : Tag(T), SerpentinaResfriamento(T + "-SERP", TipoSerpentina::RESFRIAMENTO, 20.0), Sensor(T + "-TT", TipoSensorTemp::RESERVATORIO_FRIO, 20.0) {
        SerpentinaResfriamento.Ligar(50.0);
    }

    void atualizar(double HorasCiclo) {
        double tempAtual = Sensor.getValor();

        if (tempAtual > TEMP_ALVO + TOLERANCIA) {
            //temp subiu - aumenta potencia de resfriamento
            double novaPotencia = SerpentinaResfriamento.getPotencia() + 10.0;
            SerpentinaResfriamento.AjustarPotencia(novaPotencia);
        } else if (tempAtual < TEMP_ALVO - TOLERANCIA) {
            //temp caiu - reduz potencia
            double novaPotencia  = SerpentinaResfriamento.getPotencia() - 10.0;
            SerpentinaResfriamento.AjustarPotencia(novaPotencia);
        }

        double variacao = SerpentinaResfriamento.calcularVariacaoTemperatura();
        Sensor.atualizarValorReservatorio(variacao, TEMP_ALVO);
        Sensor.simular(false, false);

        SerpentinaResfriamento.atualizarMonitor(HorasCiclo);
    }

    void simularFalhaSerpentina() {
        SerpentinaResfriamento.AtivarFalha();
        cout << "[FALHA] Serpentina do reservatório frio falhou!" << endl;
    }

    double getTemperatura()     const { return Sensor.getValor(); }
    string getTagSensor()       const { return Sensor.getTag(); }
    Serpentina& getSerpentina()       { return SerpentinaResfriamento; }
    SensorTemperatura& getSensor()    { return Sensor; }
};

//o tanque de mistura recebe água quente e fria, calcula temperatura e monitora o nível

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
    TanqueMistura(string T) : Tag(T), Nivel(50.0), Pressao(1.0), SensorTemp(T + "-TT", TipoSensorTemp::TANQUE_MISTURA, 23.0), SensorNiv(T + "-LT"), SensorPress(T + "-PT") { //construtor
    }

    //Chamado a cada ciclo com as vazões e temperatura das fontes
    //precisa saber quanta água está entrando de cada lado e se a bomba de escoamento ta ligada pra esvaziar o tanque
    void atualizar(double VazaoQuente, double VazaoFria, double TempQuente, double TempFria, bool BombaSaidaLigada, double HorasCiclo) {
        //atualiza a temperatura pela fórmula de mistura
        //se tiver água entrando entrando ele calcula a media ponderada termica e injeta no direto no sensor do tanque
        double somaVazoes = VazaoQuente + VazaoFria;
        if (somaVazoes > 0.0) {
            double tempMistura = (VazaoQuente * TempQuente + VazaoFria * TempFria) / somaVazoes;
            SensorTemp.atualizarValorMistura(tempMistura);
        }

        //atualiza o nivel -> entrada menos saida
        double entrada = (VazaoQuente + VazaoFria) * 0.02; //converte L/min em % por ciclo
        double saida = BombaSaidaLigada ? 1.0 : 0.2; //se a bomba de saida estiver ligada ele esvazia 1% por ciclo, se não, perde 0,2% por vazamento ou evaporação simulada
        Nivel += entrada - saida;
        if (Nivel > 100.0) Nivel = 100.0;
        if (Nivel < 0.0) Nivel = 0.0;

        //atualiza a pressao com base no nivel e bomba de saida
        Pressao = 1.0 + (Nivel / 100.0) * 2.0; //começa com 1atm e conforme o nivel sobe o peso da agua adiciona até mais 2 atm 
        if (BombaSaidaLigada) Pressao += 2.0; //se a bomba estiver ligada ela gera uma CONTRAPRESSAO de mais 2 atm
        if (Pressao > 10.0) Pressao = 10.0; //trava maxima de 10 atm

        //injeta os valores nos sensores
        SensorNiv.simular(somaVazoes > 0, false);
        SensorPress.simular(BombaSaidaLigada, false);
    }
//bloco de encapsulamento q contem os getters pra ler os estados sem violar o escopo privado
    double getTemperatura() const { return SensorTemp.getValor(); }
    double getNivel() const { return Nivel; }
    double getPressao() const { return Pressao; }
    double getLimiteBaixo() const { return LIMITE_NIVEL_BAIXO; }
    double getLimiteAlto() const { return LIMITE_NIVEL_ALTO; }
    double getLimiteCritico() const { return LIMITE_NIVEL_CRITICO; }
    double getLimitePressao() const { return LIMITE_PRESSAO_ALTA; }

    SensorTemperatura& getSensorTemp() { return SensorTemp; }
    SensorNivel& getSensorNivel() { return SensorNiv; }
    SensorPressao& getSensorPress() { return SensorPress; }
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

int main() {

    // Registro criado primeiro pois todos dependem dele
    RegistroSistema       registro;
    SistemaAcesso         acesso(registro);
    GerenciadorManutencao gerManutencao(registro);

    // Login obrigatório
    string nome, senha;
    cout << "=== LOGIN ===" << endl;
    cout << "Usuario: "; cin >> nome;
    cout << "Senha:   "; cin >> senha;

    if (!acesso.login(nome, senha)) {
        cout << "Login falhou. Encerrando." << endl;
        return 1;
    }

    // Configuração do processo pelo operador
    double tempDesejada, volumeDesejado;
    cout << "\n=== CONFIGURACAO DO PROCESSO ===" << endl;
    cout << "Temperatura desejada (20 a 70 C): "; cin >> tempDesejada;
    cout << "Volume por minuto desejado (L/min): "; cin >> volumeDesejado;

    // Registra a ação do operador
    registro.registrarAcaoOperador(
        acesso.getNomeAtivo(),
        "Setpoint definido: " + to_string(tempDesejada) +
        "C | Volume: " + to_string(volumeDesejado) + " L/min");

    CalculadoraDemanda::Parametros params =
        CalculadoraDemanda::calcular(tempDesejada, volumeDesejado);

    cout << "\n[SISTEMA] Parametros calculados automaticamente:" << endl;
    cout << "  Valvula quente:  " << params.AberturaValvulaQuente << "%" << endl;
    cout << "  Valvula fria:    " << params.AberturaValvulaFria   << "%" << endl;
    cout << "  Potencia bombas: " << params.PotenciaBombaQuente   << "%" << endl;

    // Equipamentos
    ParBombas parQuente("BOMBA-Q1", "BOMBA-Q2", 80.0);
    ParBombas parFria  ("BOMBA-F1", "BOMBA-F2", 80.0);

    gerManutencao.registrarMonitor(&parQuente.getPrincipal().Monitor);
    gerManutencao.registrarMonitor(&parQuente.getReserva().Monitor);
    gerManutencao.registrarMonitor(&parFria.getPrincipal().Monitor);
    gerManutencao.registrarMonitor(&parFria.getReserva().Monitor);

    Valvula valvulaQuente("FV-QUENTE");
    Valvula valvulaFria  ("FV-FRIA");
    valvulaQuente.Abrir(params.AberturaValvulaQuente);
    valvulaFria.Abrir  (params.AberturaValvulaFria);

    parQuente.Ligar(params.PotenciaBombaQuente);
    parFria.Ligar  (params.PotenciaBombaFria);

    SensorNivel       sensorNivel("LT-61");
    SensorPressao     sensorPressao("PT-61");
    SensorTemperatura sensorTemperatura("TT-61");

    NivelBaixo  regraNivelBaixo;
    NivelAlto   regraNivelAlto;
    PressaoAlta regraPressaoAlta;

    EstacaoBombeamento estacao;

    const double HORAS_POR_CICLO = 0.5;

    cout << "\n___ SIMULACAO INICIADA | Usuario: "
         << acesso.getNomeAtivo() << " ___" << endl;
    cout << "Comandos disponiveis a cada ciclo:" << endl;
    cout << "  [ENTER] = avancar ciclo" << endl;
    cout << "  s       = alterar setpoint (operador)" << endl;
    cout << "  m       = propor manutencao (tecnico)" << endl;
    cout << "  a       = aprovar manutencao (admin)" << endl;
    cout << "  r       = recusar manutencao (admin)" << endl;
    cout << "  h       = ver historico (admin)" << endl;
    cout << "  q       = encerrar" << endl;

    int ciclo = 1;
    while (true) {
        registro.setCiclo(ciclo);

        cout << "\n>>>> CICLO: " << ciclo
             << " | Usuario: " << acesso.getNomeAtivo() << endl;

        parQuente.atualizarMonitores(HORAS_POR_CICLO);
        parFria.atualizarMonitores  (HORAS_POR_CICLO);
        gerManutencao.verificarAlertas();

        sensorNivel.simular      (estacao.isBombaEntradaLigada(), estacao.isValvulaAberta());
        sensorPressao.simular    (estacao.isBombaEntradaLigada(), estacao.isValvulaAberta());
        sensorTemperatura.simular(estacao.isBombaEntradaLigada(), estacao.isValvulaAberta());

        estacao.setNivel      (sensorNivel.getValor());
        estacao.setPressao    (sensorPressao.getValor());
        estacao.setTemperatura(sensorTemperatura.getValor());

        cout << " [BOMBAS]"
             << " | Quente: " << parQuente.getTagAtiva()
             << " (" << (parQuente.estaUsandoReserva() ? "RESERVA" : "PRINCIPAL") << ")"
             << " | Fria: " << parFria.getTagAtiva()
             << " (" << (parFria.estaUsandoReserva() ? "RESERVA" : "PRINCIPAL") << ")"
             << endl;

        cout << " [VALVULAS]"
             << " | Quente: " << valvulaQuente.getAbertura() << "%"
             << " | Fria: "   << valvulaFria.getAbertura()   << "%"
             << endl;

        cout << " [LEITURAS]"
             << " | " << sensorNivel.getTag()       << ": " << sensorNivel.getValor()
             << " "   << sensorNivel.getUnidade()
             << " | " << sensorPressao.getTag()     << ": " << sensorPressao.getValor()
             << " "   << sensorPressao.getUnidade()
             << " | " << sensorTemperatura.getTag() << ": " << sensorTemperatura.getValor()
             << " "   << sensorTemperatura.getUnidade()
             << endl;

        cout << " [MONITOR BOMBA-Q1]"
             << " Horas: "    << parQuente.getPrincipal().Monitor.getHorasOperacao()
             << " | Temp: "   << parQuente.getPrincipal().Monitor.getTemperaturaOperacao()
             << "C | Status: "<< parQuente.getPrincipal().Monitor.getStatusTexto()
             << endl;

        regraNivelBaixo.aplicar (estacao);
        regraNivelAlto.aplicar  (estacao);
        regraPressaoAlta.aplicar(estacao);

        // Menu de comandos
        cout << "\nComando (ENTER para avancar): ";
        string cmd;
        cin.ignore();
        getline(cin, cmd);

        if (cmd == "q") {
            cout << "Encerrando." << endl;
            break;

        } else if (cmd == "s") {
            // Alterar setpoint - qualquer usuário logado pode fazer
            double novaTemp, novoVol;
            cout << "Nova temperatura desejada: "; cin >> novaTemp;
            cout << "Novo volume por minuto:     "; cin >> novoVol;
            params = CalculadoraDemanda::calcular(novaTemp, novoVol);
            valvulaQuente.Abrir(params.AberturaValvulaQuente);
            valvulaFria.Abrir  (params.AberturaValvulaFria);
            parQuente.AjustarPotencia(params.PotenciaBombaQuente);
            parFria.AjustarPotencia  (params.PotenciaBombaFria);
            registro.registrarAcaoOperador(
                acesso.getNomeAtivo(),
                "Setpoint alterado: " + to_string(novaTemp) +
                "C | Volume: " + to_string(novoVol) + " L/min");

        } else if (cmd == "m") {
            // Propor manutenção - exige técnico ou admin
            try {
                string equip, motivo, data;
                cout << "Equipamento: "; cin >> equip;
                cout << "Motivo: ";      cin >> motivo;
                cout << "Data prevista: "; cin >> data;
                gerManutencao.proporManutencao(equip, motivo, data, acesso);
            } catch (runtime_error& e) {
                cout << e.what() << endl;
            }

        } else if (cmd == "a") {
            // Aprovar manutenção - exige admin
            try {
                string equip;
                cout << "Equipamento para aprovar: "; cin >> equip;
                gerManutencao.aprovarManutencao(equip, acesso);
            } catch (runtime_error& e) {
                cout << e.what() << endl;
            }

        } else if (cmd == "r") {
            // Recusar manutenção - exige admin
            try {
                string equip, motivo;
                cout << "Equipamento para recusar: "; cin >> equip;
                cout << "Motivo da recusa: ";         cin >> motivo;
                gerManutencao.recusarManutencao(equip, motivo, acesso);
            } catch (runtime_error& e) {
                cout << e.what() << endl;
            }

        } else if (cmd == "h") {
            // Ver histórico - exige admin
            try {
                gerManutencao.exibirHistorico(acesso);
            } catch (runtime_error& e) {
                cout << e.what() << endl;
            }
        }

        this_thread::sleep_for(chrono::seconds(3));
        ciclo++;
    }

    return 0;
}