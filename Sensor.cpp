#include "Sensor.hpp"

Sensor::Sensor(string t, string u) : Tag(t), Unidade(u),Valor(0.0) {} // As chaves ficam vazias pois não tem nenhuma outra ação para realizar, já foi inicializado as variáveis

    // 'const' pq são funções que servem apenas para leitura de dados, sem alterações
    // 'return tag' para retornar o nome de cada sensor
    string Sensor::getTag() const {return Tag;}
    string Sensor::getUnidade() const {return Unidade;}
    double Sensor::getValor() const {return Valor;}
    
    Sensor::~Sensor() {}
