#pragma once

enum class NivelAcesso {
    OPERADOR = 1, //ajusta os setpoints, vê leituras, faz pedidos de processo e reconhece alarmes
    TECNICO = 2, //agenda manutenção, coloca equipamentos em manutenção, acessa históricos
    ADMIN = 3 //desliga tudo, cadastra usuários, altera os limites de segurança do sistema, faz tudo dos outros dois em caso de emergência, permite manutenções, emite relatórios e registros
};