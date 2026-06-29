#include "GeradorJSON.hpp"

string GeradorJSON::gerarTimestamp() {
        time_t agora = time(nullptr); //pega o tempo atual em segundos
        tm* t = localtime(&agora); //converte para dia/hora legível
        char buffer[25]; //caractere temporário pra guardar o texto
        //formata no padrão ISO 8601 que o python entende mais facil
        strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", t);
        return string(buffer); //transforma o buffer em string e devolve
    }
GeradorJSON::GeradorJSON() {
        //abre o arquivo no modo append pra não apagar leituras antigas
        Arquivo.open("leituras.json", ios::app);
    }
GeradorJSON::~GeradorJSON() {
        Arquivo.close();
    }
        
//escreve uma leitura do sensor no arquivo
void GeradorJSON::escreverLeitura(string tag, string variavel, double valor, string unidade, string status, int ciclo) {
    Arquivo << "{" << "\"tipo\":\"leitura\"," << "\"sensor\":\"" << tag << "\"," << "\"valor\":" << valor << "," << "\"unidade\":\"" << unidade << "\"," << "\"status\":\"" << status << "\"," << "\"timestamp\":\"" << gerarTimestamp() << "\"," << "\"ciclo\":" << ciclo << "}" << endl;    }

// Escreve o estado de um atuador (bomba ou válvula)
void GeradorJSON::escreverAtuador(string nome, string estado, double valor, int ciclo) {
    Arquivo << "{" << "\"tipo\":\"atuador\"," << "\"nome\":\"" << nome << "\"," << "\"estado\":\"" << estado << "\"," << "\"valor\":" << valor << "," << "\"timestamp\":\"" << gerarTimestamp() << "\"," << "\"ciclo\":" << ciclo << "}" << endl;
}

//escreve um alarme ativo
void GeradorJSON::escreverAlarme(string codigo, string severidade, int ciclo) {
    Arquivo << "{" << "\"tipo\":\"alarme\"," << "\"codigo\":\"" << codigo << "\","<< "\"severidade\":\"" << severidade << "\"," << "\"timestamp\":\"" << gerarTimestamp() << "\"," << "\"ciclo\":" << ciclo<< "}" << endl;
}

//escreve um comando executado pelo operador
void GeradorJSON::escreverComando(string nomeComando, string usuario, int ciclo) {
        Arquivo << "{" << "\"tipo\":\"comando\"," << "\"nome\":\"" << nomeComando << "\"," << "\"usuario\":\"" << usuario << "\"," << "\"timestamp\":\"" << gerarTimestamp() << "\"," << "\"ciclo\":" << ciclo << "}" << endl;
    }
    