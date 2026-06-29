# TRABALHO_POO
Repositório criado para o trabalho de Programação Orientada a Objetos desenvolvido em dupla para fins acadêmicos

DESENVOLVEDORES:
ISABELA GIMENES MAGALHÃES RIBEIRO _ 20251CECA0369
STEPHANY BARBOSA AMARAL _ 20251CECA0091

                                DESCRIÇÃO DO PROJETO
Este projeto é um simulador de um sistema SCADA (Supervisory Control and Data Acquisition) desenvolvido em C++ para uma Planta de Mistura de Água Quente e Fria. O sistema simula o chão de fábrica (sensores, válvulas e bombas), o processamento automatizado (regras de controle) e a administração industrial (gestão de manutenções, níveis de acesso e histórico de alarmes).
O simulador é interativo e funciona via terminal, permitindo que utilizadores com diferentes "crachás" (níveis de acesso) operem a planta, aprovem manutenções e visualizem relatórios.


                                PRINCIPAIS FUNCIONALIDADES
- Automação Inteligente: Cálculo automático da abertura das válvulas e potência das bombas para atingir a temperatura desejada pelo operador.

- Física Simulada: Sensores de Nível e Pressão que simulam "ruído" no mundo real (Random Walk) e atuadores (válvulas) que respeitam limites de abertura (0 a 100%).

- Redundância Industrial (ParBombas): Sistema failover com bombas principal e reserva. Se a principal falhar, a reserva assume automaticamente.

- Manutenção Preditiva: Monitorização de desgaste, horas de uso e ciclos de liga/desliga. O sistema gera alertas quando o equipamento precisa de revisão.

- Controle de Acesso (RBAC): Sistema de Login com 3 níveis hierárquicos: Operador (1), Técnico (2) e Administrador (3).

- Diário de Bordo (Arquivos .txt): Persistência de dados usando a biblioteca <fstream>. Todo o histórico de ações e alarmes é guardado em disco.


                                CONCEITOS DE POO APLICADOS
Durante o desenvolvimento, aplicamos os pilares fundamentais da Programação Orientada a Objetos:
- Encapsulamento: Proteção das variáveis físicas (como a abertura da válvula ou as passwords dos utilizadores) usando os modificadores private e protected, com acesso controlado via métodos getters e setters.
- Herança: A classe base Sensor partilha as suas características com as classes filhas (SensorNivel, SensorPressao, etc.).
- Polimorfismo: A função virtual pura simular() na classe base obriga cada sensor específico a calcular a sua própria simulação de forma única usando o override.
- Composição: A Estação de Bombeamento/Planta é composta por objetos mais pequenos (instâncias de Válvulas, Bombas e Sensores).
- Tratamento de Exceções (<stdexcept>): Utilização de try-catch e throw runtime_error para bloquear utilizadores sem permissão sem "crashar" o programa inteiro.


                                ESPECIFICAÇÕES TÉCNICAS E PARÂMETROS ADOTADOS
Para simular uma planta industrial realista, definimos valores e limites de segurança. O sistema toma decisões automáticas baseadas nestes parâmetros:

1. Limites do Tanque de Mistura
- Capacidade de Operação: 0.0% a 100.0%.
- Nível Baixo de Segurança (LIMITE_BAIXO): 27.5%.
- Regra de Controle: Se o nível cair abaixo deste valor, o CLP liga automaticamente a bomba de entrada e fecha a válvula de saída para reabastecer.
- Alarme de Processo: Se o nível for <= 27.5% e o operador tentar extrair líquido para o processo, o sistema bloqueia a ação (lançando um runtime_error) e informa o nível extra necessário.
- Nível Alto de Segurança (LIMITE_ALTO): 82.5%.
- Regra de Controle: Se atingir este valor, a bomba de entrada é desligada por segurança para evitar transbordamento, e a válvula de escoamento é aberta.

2. Limites de Pressão
- Pressão de Operação: 0.0 a 10.0 bar.
- Pressão Alta (PRESSAO_ALTA): 6.5 bar.
- Regra de Controle: Ao atingir ou superar 6.5 bar, a bomba de entrada é imediatamente desligada para aliviar a tubagem e evitar ruturas.

3. Setpoints de Temperatura
- Limites Extremos do Fluido: Temperatura Baixa (15.0 °C) e Temperatura Alta (73.0 °C).
- Regulação Automática: O operador define um setpoint (ex: 45.0 °C) e o algoritmo calcula as frações exatas necessárias (abertura percentual da válvula quente vs. válvula fria) para atingir e estabilizar essa temperatura.

4. Limites de Manutenção Preditiva
Os motores e bombas possuem sensores virtuais de fadiga. O status muda para "ATENÇÃO" se algum destes limites industriais for atingido:
- Vida Útil Total: 2000.0 horas de operação.
- Desgaste por Esforço: 500.0 horas operando em Potência Alta (>80%).
- Temperatura Crítica do Motor: Operação do equipamento acima de 80.0 °C (podendo chegar a um pico de segurança forçado de 95°C).
- Fadiga Mecânica: Excesso de ciclos Liga/Desliga (1000 ciclos).


                                COMO USAR (MENU INTERATIVO)
Ao iniciar o programa, será apresentado um menu interativo no terminal.
Recomenda-se começar por fazer Login [L] para ter acesso às funções restritas.
Utilizadores pré-cadastrados para teste:
- Administrador: Usuário admin | Senha admin123
- Técnico: Usuário tecnico | Senha tec456
- Operador: Usuário operador | Senha op789

## COMO COMPILAR E EXECUTAR

**Pré-requisitos:** g++ com suporte a C++17 e Python 3 com pip instalado.

    **C++ (simulador do chão de fábrica):**
        g++ *.cpp -o isaste -std=c++17
        ./isaste
    **Supervisório Python (interface web):**
        pip install streamlit pandas
        python3 -m streamlit run supervisor.py

**Ordem de execução recomendada:**
    1. Compile e inicie o C++ primeiro
    2. Faça login com um dos usuários de teste
    3. Digite `p` no terminal para ativar o modo automático
    4. Em outro terminal, inicie o Streamlit
    5. Acesse `http://localhost:8501` no navegador

## ARQUIVOS GERADOS PELO SISTEMA

Durante a execução, o programa cria e atualiza automaticamente os seguintes arquivos na pasta do projeto:

- `leituras.json` — leituras dos sensores em tempo real, consumido pelo Streamlit
- `comandos.json` — comandos enviados pelo supervisório ao C++
- `historico.csv` — histórico completo de leituras exportável
- `historico_alarmes.txt` — registro permanente de todos os alarmes disparados
- `historico_acoes.txt` — registro permanente de todas as ações dos operadores
