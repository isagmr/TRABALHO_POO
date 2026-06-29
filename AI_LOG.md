# 🤖 Registro de Uso de Inteligência Artificial (AI_LOG)

- Ferramenta Utilizada: Google Gemini
- Período de Uso: Durante a fase de codificação, depuração e integração do projeto final.
- Objetivo Principal: Atuar como assistente de sintaxe e depuração. A dupla chegou às sessões com a lógica industrial já mapeada no papel e a arquitetura de classes já definida, usando a IA para resolver barreiras específicas de implementação em C++.

## 📐 O que veio da dupla (antes de qualquer IA)

Antes de escrever a primeira linha de código com auxílio de IA, a dupla definiu:

- O tema da planta de mistura de água quente e fria e a lógica de funcionamento dos equipamentos
- A arquitetura de classes no papel: quais seriam as classes, o que cada uma guardaria e como se relacionariam
- A escolha dos padrões de projeto Strategy (para as regras de controle) e Command (para os comandos do operador)
- Os parâmetros industriais: limites de 27.5% e 82.5% de nível, 6.5 bar de pressão, 2000 horas de vida útil dos equipamentos
- A decisão de integrar um supervisório Python via Streamlit lendo arquivos JSON gerados pelo C++
- O sistema de níveis de acesso RBAC com três perfis: Operador, Técnico e Administrador

## 📝 Resumo Detalhado das Interações

### 1. Tradução da Física de Atuadores para Código
- A nossa ideia: Sabíamos que uma válvula proporcional não vai de 0% a 100% instantaneamente e queríamos separar a "abertura alvo" da "abertura física", simulando a inércia real do equipamento.
- O desafio prático: Como criar um motor de simulação que atualizasse isso progressivamente a cada ciclo do loop sem travar o terminal?
- Solução com IA: A IA auxiliou na estruturação do método `simular()`, mostrando como implementar incrementos parciais dentro do loop de controle respeitando o encapsulamento dos atributos privados.

### 2. Implementação do Ruído nos Sensores (Random Walk)
- A nossa ideia: Queríamos aplicar o conceito de caminhada aleatória para simular o ruído natural dos sensores físicos, somando ou subtraindo pequenos valores da leitura real a cada ciclo.
- O desafio prático: Não tínhamos experiência com geração de números estocásticos em C++.
- Solução com IA: A IA explicou a sintaxe do `std::uniform_real_distribution` da biblioteca `<random>` e ajudou a depurar um erro causado pela ordem dos parâmetros (menor vs. maior) na função de distribuição.

### 3. Gestão de Memória e Herança no C++
- A nossa ideia: Arquitetamos a árvore de herança com uma classe base `Sensor` e classes filhas como `SensorNivel`, `SensorPressao` e `SensorTemperatura`, usando polimorfismo para que cada sensor se simulasse de forma diferente.
- O desafio prático: Desconhecíamos algumas regras de gestão de memória do C++ ao usar objetos polimórficos.
- Solução com IA: Ao pedirmos para a IA revisar nossa hierarquia, ela alertou para a necessidade do destrutor virtual `virtual ~Sensor()`. Aprendemos que, sem essa sintaxe, o C++ não liberaria corretamente a memória das classes filhas, causando memory leaks.

### 4. Integração de Módulos (Ponteiros e Referências)
- A nossa ideia: Projetamos o `SistemaAcesso`, o `GerenciadorManutencao` e o `RegistroSistema` como módulos separados que se conectariam sem duplicar dados em memória.
- O desafio prático: Como conectar essas classes diferentes sem criar cópias desnecessárias dos objetos?
- Solução com IA: A IA explicou como passar referências corretamente nos construtores, como em `SistemaAcesso(RegistroSistema& R) : Registro(R)`, e como usar ponteiros `Usuario*` para rastrear o usuário ativo sem duplicar dados.

### 5. Separação em arquivos .hpp e .cpp
- A nossa ideia: Organizar o projeto nos padrões industriais, separando declaração e implementação em arquivos distintos.
- O desafio prático: A separação gerou dezenas de erros em cascata relacionados a includes incorretos, construtores com inicializadores mal posicionados e qualificações de classe desnecessárias.
- Solução com IA: A IA ajudou a identificar os padrões de erro. Vale registrar que nem sempre a primeira solução sugerida funcionou — foi necessário iterar várias vezes, o que mostrou que a IA não substitui o entendimento do código, apenas acelera a depuração.

### 6. Supervisório Python (Streamlit)
- A nossa ideia: Criar uma interface web de supervisão industrial que lesse os dados do C++ em tempo real via arquivos JSON, com cards coloridos de alarme, histórico de comandos e gestão de manutenção.
- O desafio prático: Estruturar o código Streamlit com múltiplas colunas, atualização automática e lógica de permissões por perfil de usuário.
- Solução com IA: A IA auxiliou na estrutura do layout e na leitura dos arquivos JSON com pandas, enquanto a lógica de negócio, as permissões e a comunicação via JSON foram definidas pela dupla.

## ⚠️ Limitações encontradas no uso da IA

- A IA cometeu erros ao sugerir a separação em `.hpp` e `.cpp`, gerando includes circulares e construtores mal declarados que precisaram de várias rodadas de correção
- Em vários casos a IA não reconheceu que o problema estava em um arquivo diferente do que estava sendo analisado, exigindo que a dupla identificasse a origem real do erro
- A IA não conhecia o contexto completo do projeto, então foi sempre necessário guiar as perguntas com detalhes específicos para obter respostas úteis

## 💬 Exemplos Reais de Prompts Utilizados

- "Nós mapeamos uma classe RegraControle base para usar o padrão Strategy. Qual é a sintaxe exata em C++ para criar uma função virtual pura de modo que as classes filhas sejam obrigadas a ter o método aplicar()?"
- "Queremos criar um ruído realista para o sensor. Já conhecemos a lógica do Random Walk, mas como implementamos isso usando a biblioteca `<random>` do C++?"
- "Temos a lógica de permissões e login desenhada, mas estamos com problemas na integração. Como funciona a passagem de referências na classe SistemaAcesso para que o objeto de registro escreva os dados no arquivo sem criar novas instâncias na memória?"
- "Sabemos que precisamos de polimorfismo aqui, mas por que o compilador exige um `virtual ~Sensor()`? Qual é o impacto disso na memória?"

## 🎯 Conclusão

A IA foi usada como ferramenta de apoio técnico, não como autora do projeto. As decisões de arquitetura, os padrões de projeto escolhidos, a lógica industrial simulada e os parâmetros adotados são produtos do trabalho da dupla. A IA resolveu barreiras de sintaxe e depuração que, sem ela, demandariam mais tempo de pesquisa em documentação, mas não substituiu o raciocínio por trás do sistema. O fato de que várias sugestões da IA precisaram ser corrigidas ou descartadas reforça que o controle do projeto esteve sempre nas mãos da dupla.