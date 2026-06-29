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

#include "NivelAcesso.hpp"

using namespace std;

class Usuario{
    private:
    string Nome;
    string Senha;
    NivelAcesso Nivel;

    public:
    Usuario(string N, string S, NivelAcesso Nv);
    bool verificarSenha(string S) const;
    bool temPermissao(NivelAcesso NivelNecessario) const;

    string getNome() const;
    NivelAcesso getNivel() const;

    //'traduz' o nivel numerico para texto
    string getNivelTexto() const;
};