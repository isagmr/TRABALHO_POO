#include "SistemaAcesso.hpp"

SistemaAcesso::SistemaAcesso(RegistroSistema& R) : Registro(R) { //: Registro(R) é uma lista de inicializalçao; como Registro é uma referência e uma referência não pode
    //nascer vazia, então ela se conecta ao objeto Regitro que nasceu na main e o R é só um apelido que transporta o objeto da main pra dentro da classe
    UsuarioAtivo = nullptr; //começa apontando pro nada
    Usuarios.push_back(Usuario("admin", "admin123", NivelAcesso::ADMIN)); //push_back é adicionar ao final da lista
    Usuarios.push_back(Usuario("tecnico", "tec456", NivelAcesso::TECNICO));
    Usuarios.push_back(Usuario("operador", "op789", NivelAcesso::OPERADOR));
}

bool SistemaAcesso::login(string Nome, string Senha) {
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

void SistemaAcesso::logout() {
    if (UsuarioAtivo != nullptr) {
        cout << "[ACESSO] Logout: " << UsuarioAtivo->getNome() << endl; //como UsuarioAtivo é um ponteiro, não da pra usar . pra chamar, por isso o uso da seta ->
        //ele vai até o endereço e chama a função getNome que tem lá dentro
    }
    UsuarioAtivo = nullptr; //pra deslogar o usuário o ponteiro volta a apontra pro vazio
}

bool SistemaAcesso::estaLogado() const {
    return UsuarioAtivo != nullptr; //se o ponteiro for diferente de vazio, tem um endereço guaraddomali, então alguem está logado
}

//função que valida o acesso
void SistemaAcesso::exigirPermissao(NivelAcesso NivelNecessario) const {
    if (UsuarioAtivo == nullptr) {
        //throw runtime_error é uma clausula de guarda que joga um erro na tela e para o processo;vem da biblioteca stdexcept; throw é um try-catch
        throw runtime_error("ACESSO NEGADO: Nenhum usuário logado.");
    }
    if (!UsuarioAtivo->temPermissao(NivelNecessario)) {
        throw runtime_error("ACESSO NEGADO: " + UsuarioAtivo->getNome() + " não tem permissão para esta operação.");
    }
}

//acessa quem está logado no momento
string SistemaAcesso::getNomeAtivo() const {
    if (UsuarioAtivo == nullptr) return "Ninguém"; //devolve 'ninguem' caso n tenha ninguem logado
    return UsuarioAtivo->getNome();
}

//se o ponteiro for inválido (ninguem logado) a resposta padrão é operador
NivelAcesso SistemaAcesso::getNivelAtivo() const {
    return UsuarioAtivo ? UsuarioAtivo->getNivel() : NivelAcesso::OPERADOR;
}
