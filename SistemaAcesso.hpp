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

#include "Usuario.hpp"
#include "RegistroSistema.hpp"
#include "NivelAcesso.hpp"
#include <vector>

using namespace std;

class SistemaAcesso {
    private:
    vector<Usuario> Usuarios; //é como uma lista dinâmica (um array que pode crescer) que armazena os usuarios cadastrados
    Usuario* UsuarioAtivo; //é um ponteiro que aponta diretamente pro usuario que acabou de fazer login. se ninguem estiver logado, ele aponta pro vazio 
    RegistroSistema& Registro; //é uma referência; em vez de criar um sistema de registro novo, ele conecta diretamnete ao historico central criado antes, é assim q o sistema de login vai gravar coisas no arquivo de texto
    public:
    SistemaAcesso(RegistroSistema& R);
    bool login(string Nome, string Senha);
    void logout();
    bool estaLogado() const;
    void exigirPermissao(NivelAcesso NivelNecessario) const;
    string getNomeAtivo() const;
    NivelAcesso getNivelAtivo() const;
};