#include "Usuario.hpp"

Usuario::Usuario(string N, string S, NivelAcesso Nv) {
    Nome = N;
    Senha = S;
    Nivel = Nv;
}

bool Usuario::verificarSenha(string S) const {
    return Senha == S;
}

//transforma temporariamente as variaveis no tipo inteiro; a lógica é: cada nivel de acesso é representado por um numero, se o usuario logado for um
//tecnico (2) e ele tentar apertar um botao que exige ser admin(3), a função faz a conversão e testa se 2 é >= 3, como não é, ela retorna
//falso e barra o acesso do técnico
bool Usuario::temPermissao(NivelAcesso NivelNecessario) const {
    return static_cast<int>(Nivel) >= static_cast<int>(NivelNecessario);
}

string Usuario::getNome() const { return Nome; }
NivelAcesso Usuario::getNivel() const { return Nivel; }

//'traduz' o nivel numerico para texto
string Usuario::getNivelTexto() const {
    if (Nivel == NivelAcesso::ADMIN) return "Administrador";
    if (Nivel == NivelAcesso::TECNICO) return "Técnico";
    return "Operador";
}