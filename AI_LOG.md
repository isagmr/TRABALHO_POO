🤖 Registro de Uso de Inteligência Artificial (AI_LOG)
- Ferramenta Utilizada: Google Gemini
- Período de Uso: Durante a fase de codificação, depuração e integração do projeto final.
- Objetivo Principal: Atuar como um "assistente de sintaxe e implementação". A dupla possuía o mapeamento claro da lógica industrial e da arquitetura Orientada a Objetos (POO) que desejava aplicar, mas utilizou a IA para transpor essa lógica teórica para a sintaxe complexa e as boas práticas da linguagem C++.

📝 Resumo Detalhado das Interações
A nossa dinâmica de trabalho com a IA baseou-se em apresentar o nosso planeamento estrutural e pedir auxílio para a codificação em C++. As interações focaram-se nas seguintes barreiras de implementação:

1. Tradução da Física de Atuadores para Código
- A Nossa Ideia: Sabíamos que uma válvula proporcional não vai de 0% a 100% instantaneamente e queríamos separar a "abertura alvo" da "abertura física".
- O Desafio Prático: Como criar um motor de simulação que atualizasse isso progressivamente a cada ciclo do loop sem travar o terminal?
- Solução com IA: A IA auxiliou-nos na estruturação do método simular(), mostrando como implementar incrementos parciais (ex: AberturaAtual += 10.0) dentro de um loop de controle no C++, respeitando o encapsulamento dos nossos atributos.

2. Implementação do Ruído nos Sensores (Random Walk)
- A Nossa Ideia: Queríamos aplicar o conceito de caminhada aleatória para simular o ruído natural dos sensores físicos, somando ou subtraindo pequenos valores da leitura real.
- O Desafio Prático: Não tínhamos experiência com a geração de números estocásticos avançados em C++.
- Solução com IA: Usámos a IA para entender especificamente como declarar e utilizar a biblioteca <random>. A IA mostrou-nos a sintaxe do std::uniform_real_distribution e ajudou a depurar um erro de compilação causado pela ordem dos parâmetros (menor vs. maior) na função.

3. Gestão de Memória e Herança no C++
- A Nossa Ideia: Arquitetámos perfeitamente a árvore de herança (uma classe base Sensor e classes filhas como SensorNivel) e sabíamos que queríamos usar Polimorfismo para que cada sensor se simulasse de forma diferente.
- O Desafio Prático: Desconhecíamos algumas regras estritas de gestão de memória do C++ ao usar objetos polimórficos.
- Solução com IA: Ao pedirmos para a IA rever a nossa hierarquia, ela alertou-nos para a necessidade do destrutor virtual (virtual ~Sensor()). Aprendemos que, sem essa sintaxe específica, o C++ não libertaria corretamente a memória das classes filhas, causando Memory Leaks.

4. Integração de Módulos (Ponteiros e Referências)
- A Nossa Ideia: Projetámos um sistema de Manutenção Preditiva e Controle de Acesso (RBAC) onde um Histórico Geral gravaria todas as ações.
- O Desafio Prático: Como conectar classes diferentes (o SistemaAcesso, o GerenciadorManutencao e o RegistroSistema) sem duplicar dados e estourar a memória?
- Solução com IA: Sabíamos que a resposta envolvia ponteiros, mas a sintaxe estava a gerar erros. A IA atuou como revisora, explicando como passar referências (&) corretamente nos construtores (ex: SistemaAcesso(RegistroSistema& R) : Registro(R)) e como utilizar ponteiros (Usuario*) para rastrear o utilizador ativo sem criar cópias desnecessárias.

5. Documentação e Transparência
- As documentações finais (como o README.md e este próprio AI_LOG.md) foram elaboradas em conjunto com a IA. Após a conclusão do código estrutural, forneci à ferramenta o escopo do nosso trabalho, os parâmetros de engenharia adotados (como os setpoints de 27.5% e 82.5%) e as minhas reflexões sobre o processo de aprendizagem, para que a IA auxiliasse na formatação em Markdown, garantindo o rigor técnico exigido num relatório académico.

💬 Exemplos Reais de Prompts Utilizados pela Dupla
Abaixo estão exemplos de como guiámos a IA, mostrando que tínhamos a base teórica e precisávamos da aplicação prática:

- "Nós mapeámos uma classe RegraControle base para usar o padrão Strategy. Qual é a sintaxe exata em C++ para criar uma função virtual pura de modo que as classes filhas sejam OBRIGADAS a ter o método aplicar()?"
- "Queremos criar um ruído realista para o sensor. Já conhecemos a lógica do Random Walk, mas como implementamos isso de forma correta e otimizada usando a biblioteca <random> do C++?"
- "Temos a lógica de permissões e login desenhada, mas estamos com problemas na integração. Como funciona exatamente a passagem de referências na classe SistemaAcesso para que o objeto de registo escreva os dados no arquivo .txt sem criar novas instâncias na memória?"
- "Sabemos que precisamos de polimorfismo aqui, mas porquê o compilador exige um virtual ~Sensor()? Qual é o impacto disso na memória?"

🎯 Conclusão sobre o Impacto da IA
- A IA foi fundamental para transpor a barreira entre a Engenharia de Software (planeamento e arquitetura) e a Ciência da Computação (sintaxe e gestão de memória). Ao utilizá-la para esclarecer particularidades do C++ e depurar erros específicos de compilação, conseguimos dar vida às lógicas de automação e burocracia industrial que havíamos idealizado, garantindo um código robusto, funcional e perfeitamente alinhado com as boas práticas exigidas.