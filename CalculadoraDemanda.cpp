#include "CalculadoraDemanda.hpp"


    //recebe o que o operador quer e devolve o que o sistema deve fazer; temDesejada é a temperatura em °C que o operador pediu e volumePorMinuto é a vazão de saída desejada em l/min
    CalculadoraDemanda::Parametros CalculadoraDemanda::calcular(double TempDesejada, double VolumePorMinuto) {
    CalculadoraDemanda::Parametros p;

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