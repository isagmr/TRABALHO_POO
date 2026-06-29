#include "Serpentina.hpp"

    Serpentina::Serpentina(string T, TipoSerpentina Tp, double TempAlvo) : Monitor(T) { //construtor; passamos a tag, o tipo e a temperatura alvo; ': Monitor(T)' cria o monitoramneto da saúde dela passando o nome automaticamente
       //configurando os valores iniciais
        Tag = T;
        Tipo = Tp;
        Potencia = 0.0;
        Ligada = false;
        Falha = false;
        TemperaturaAlvo = TempAlvo;
    }

    void Serpentina::Ligar(double PotenciaDesejada) {
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

    void Serpentina::AjustarPotencia(double NovaPotencia) {
        if (Falha || !Ligada) return;
        //atualiza o valor e define os limites
        Potencia  = NovaPotencia;
        if (Potencia > 100.0) Potencia = 100.0;
        if (Potencia < 0.0) Potencia = 0.0;
    }

    void Serpentina::Desligar() {
        Ligada = false;
        Potencia = 0.0;
    }

    void Serpentina::AtivarFalha() { //força uma pane no equipamento; simula acidente ou quebra; desliga tudo e fica travada em estado de erro
        Falha = true;
        Ligada = false;
        Potencia = 0.0;
    }

    //calcula quanto a serpentina consegue mudar a temperatura do reservatório a cada ciclo baseado na sua potencia atual
    double Serpentina::calcularVariacaoTemperatura() const {
        if (!Ligada || Falha) return 0.0; //se estiver desligada ou quebrada, n esquenta nem esfria

        //100% de potencia causa variação máxima de 2 graus por ciclo; então a variação é proporcional a potencia atual
        double variacaoMaxima = 2.0;
        if (Tipo == TipoSerpentina::AQUECIMENTO) {
            return (Potencia / 100.0) * variacaoMaxima; //positico aquece
        } else {
            return -(Potencia /100.0) * variacaoMaxima; //negativo resfria
        }
    }

    void Serpentina::atualizarMonitor(double HorasCiclo) { //repassando os dadsos pro monitor calcular o desgaste termico a cada ciclo
        Monitor.atualizar(Ligada, Potencia, HorasCiclo);
    }

    //bloco de funlções get que só entrgam cópias dos valores privados, com o const garantindo que os dados não vão ser alterados
    bool Serpentina::estaLigada() const { return Ligada; }
    bool Serpentina::temFalha() const { return Falha; }
    double Serpentina::getPotencia() const { return Potencia; }
    double Serpentina::getTemperaturaAlvo() const { return TemperaturaAlvo; }
    string Serpentina::getTag() const { return Tag; }
    TipoSerpentina Serpentina::getTipo() const { return Tipo; }