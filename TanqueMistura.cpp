#include "TanqueMistura.hpp"

TanqueMistura::TanqueMistura(string T) : Tag(T), Nivel(50.0), Pressao(1.0), SensorTemp(T + "-TT", TipoSensorTemp::TANQUE_MISTURA, 23.0), SensorNiv(T + "-LT"), SensorPress(T + "-PT") { //construtor
    }

    //Chamado a cada ciclo com as vazões e temperatura das fontes
    //precisa saber quanta água está entrando de cada lado e se a bomba de escoamento ta ligada pra esvaziar o tanque
    void TanqueMistura::atualizar(double VazaoQuente, double VazaoFria, double TempQuente, double TempFria, bool BombaSaidaLigada, double HorasCiclo) {
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
    double TanqueMistura::getTemperatura() const { return SensorTemp.getValor(); }
    double TanqueMistura::getNivel() const { return Nivel; }
    double TanqueMistura::getPressao() const { return Pressao; }
    double TanqueMistura::getLimiteBaixo() const { return LIMITE_NIVEL_BAIXO; }
    double TanqueMistura::getLimiteAlto() const { return LIMITE_NIVEL_ALTO; }
    double TanqueMistura::getLimiteCritico() const { return LIMITE_NIVEL_CRITICO; }
    double TanqueMistura::getLimitePressao() const { return LIMITE_PRESSAO_ALTA; }

    SensorTemperatura& TanqueMistura::getSensorTemp() { return SensorTemp; }
    SensorNivel& TanqueMistura::getSensorNivel() { return SensorNiv; }
    SensorPressao& TanqueMistura::getSensorPress() { return SensorPress; }