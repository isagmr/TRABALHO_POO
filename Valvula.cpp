#include "Valvula.hpp"

    Valvula::Valvula(string T){
        Tag = T;
        Abertura = 0.0;
        Travada = false;
    }

    void Valvula::Abrir(double Percentual){
        if(Travada) return;   // Se a válvula estiver travada não muda nada
        
        Abertura = Percentual;
        
        if(Abertura > 100.0) Abertura = 100.0;
        if (Abertura < 0.0) Abertura = 0.0;
    }

    void Valvula::Fechar(){
        if (!Travada) Abertura = 0.0;
    }

    void Valvula::Travar() { Travada = true; }
    void Valvula::Destravar() { Travada = false; }

    double Valvula::getAbertura() const { return Abertura; }
    bool Valvula::estaTravada() const { return Travada; }
    string Valvula::getTag() const {return Tag; }