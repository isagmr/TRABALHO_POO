#include "GerenciadorComandos.hpp"

GerenciadorComandos::GerenciadorComandos(std::string arquivo){
    nome_arquivo = arquivo;
    setpoint_temp = 50.0;     // Temperatura segura de partida
    vazao_alvo = 2.0;         // Vazão segura de partida
    override_bomba_q = false; // Bombas iniciam em modo automático (desligado)
    override_bomba_f = false; 
    parada_emergencia = false;// O sistema inicia operante
}

void GerenciadorComandos::limparTexto(std::string &valor){
    std::string limpo = "";
        for (char c : valor) {
            // Se o caractere não for vírgula, nem aspas duplas, nem espaço, nós o guardamos
            if (c != ',' && c != '"' && c != ' ') {
                limpo += c;
            }
        }
        valor = limpo; // Substitui o texto sujo pelo texto limpo
    }

void GerenciadorComandos::atualizarComandos() {
        // Cria o objeto 'arquivo' e tenta abrir o comandos.json
        std::ifstream arquivo(nome_arquivo);
        
        // Proteção: Se o arquivo não existir (o site ainda não mandou nada),
        // encerra a função silenciosamente e mantém os valores antigos.
        if (!arquivo.is_open()) {
            return; 
        }

        std::string linha;

        // O comando getline vai ler o arquivo de texto linha por linha até o fim
        while (std::getline(arquivo, linha)) {
            
            // 1. VERIFICAÇÃO DO SETPOINT
            // O std::string::npos significa "Não Encontrado" (No Position).
            // Se a busca for DIFERENTE de não encontrado, significa que achamos a palavra!
            if (linha.find("\"setpoint_temp\"") != std::string::npos) {
                size_t pos = linha.find(":"); // Acha a posição do caractere dois pontos (:)
                std::string valorStr = linha.substr(pos + 1); // Corta a linha pegando tudo após os ':'
                limparTexto(valorStr); // Limpa aspas e vírgulas
                setpoint_temp = std::stod(valorStr); // std::stod converte a String Para Double
            }
            // 2. VERIFICAÇÃO DA VAZÃO
            else if (linha.find("\"vazao_alvo\"") != std::string::npos) {
                size_t pos = linha.find(":");
                std::string valorStr = linha.substr(pos + 1);
                limparTexto(valorStr);
                vazao_alvo = std::stod(valorStr); // Converte para decimal
            }
            // 3. VERIFICAÇÃO DO COMANDO MANUAL DA BOMBA QUENTE
            else if (linha.find("\"override_bomba_q\"") != std::string::npos) {
                size_t pos = linha.find(":");
                std::string valorStr = linha.substr(pos + 1);
                limparTexto(valorStr);
                // Se a string contiver a palavra "true", a variável booleana vira verdadeira
                override_bomba_q = (valorStr == "true"); 
            }
            // 4. VERIFICAÇÃO DO COMANDO MANUAL DA BOMBA FRIA
            else if (linha.find("\"override_bomba_f\"") != std::string::npos) {
                size_t pos = linha.find(":");
                std::string valorStr = linha.substr(pos + 1);
                limparTexto(valorStr);
                override_bomba_f = (valorStr == "true");
            }
            // 5. VERIFICAÇÃO DA PARADA DE EMERGÊNCIA
            else if (linha.find("\"parada_emergencia\"") != std::string::npos) {
                size_t pos = linha.find(":");
                std::string valorStr = linha.substr(pos + 1);
                limparTexto(valorStr);
                parada_emergencia = (valorStr == "true");
            }
        }
        arquivo.close(); 
    }

    double GerenciadorComandos::getSetpointTemp() const    { return setpoint_temp; }
    double GerenciadorComandos::getVazaoAlvo() const       { return vazao_alvo; }
    bool GerenciadorComandos::getOverrideBombaQ() const    { return override_bomba_q; }
    bool GerenciadorComandos::getOverrideBombaF() const    { return override_bomba_f; }
    bool GerenciadorComandos::getParadaEmergencia() const  { return parada_emergencia; }