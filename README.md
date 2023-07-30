# Busca Local Iterada Aplicada ao Sequenciamento de Tarefas em Máquinas Flexíveis Paralelas Não Idênticas

Autor : **Pedro Lucas Damasceno** (pedro.dmscno@gmail.com) - Universidade Federal de Ouro Preto  
Orientador : **Prof. Dr. Marco Antonio Moreira de Carvalho** (mamc@ufop.edu.br)  - Universidade Federal de Ouro Preto 

## Resumo

O sequenciamento de tarefas em máquinas flexíveis paralelas não idênticas (_job sequencing and
tool switching problem with non-identical parallel machines_, SSP-NPM) é um problema que con-
siste em designar e sequenciar um conjunto de tarefas às máquinas de um sistema de manufatura
flexível (_flexible manufacturing system_, FMS). Um FMS é caracterizado pela união de máquinas
flexíveis através de uma linha de produção automatizada. Cada máquina é equipada com um
magazine de capacidade limitada, que deve comportar ferramentas o suficiente para a realização
de qualquer tarefa individualmente. Uma tarefa é definida como o processo de fabricação de
um produto, que requer o acoplamento imediato de várias ferramentas, como parafusadeiras,
lixadeiras e outras, à máquina durante a sua execução. Comumente é impossível comportar simul-
taneamente todas as ferramentas do sistema, o que faz necessário a interrupção das máquinas para
a realização das trocas necessárias a fim de dar sequência ao processo de produção. Este estudo
descreve o desenvolvimento e a aplicação da metaheurística busca local iterada, combinada com
as buscas locais estruturadas no formato de descida em vizinhança variável, para a resolução do
SSP-NPM. Neste trabalho são abordados, separadamente, os objetivos de minimização de trocas
de ferramentas, _makespan_ (maior tempo decorrido, dentre todas as máquinas, desde o início da
operação até o término da última tarefa processada) e _flowtime_ (soma dos tempos de conclusão
de todas as tarefas). Resultados melhores ou equivalentes foram obtidos, em comparação ao atual
estado da arte, para todos os objetivos sobre o conjunto de 640 instâncias da literatura.

Palavras-chave: Sequenciamento. Máquinas Flexíveis. Busca Local Iterada.

## Desenvolvimento

Este trabalho combina a metaheurística ILS – uma técnica que consiste em gerar uma
solução inicial e realizar buscas locais e perturbações a fim de abrangir diferentes vizinhanças
até que se atinja o critério de parada especificado – ao VND, que explora a vizinhança da solução
atual para encontrar ótimos locais. 
  
**Operadores** :
* _job exchange_
* _job insertion_
* _2-opt_
* _1-block grouping_

Os parâmetros foram calibrados de acordo com testes estatísticos realizados pelo pacote iRace, e os experimentos foram limitados a 3.600 segundos de execução.

## Implementação

Utilizar o makefile na pasta `./imp` para compilar.

| Flag  | Parâmetro | Função |
|:---------------- |:-------------:|:-------------:
| **objective**      | {1,2,3}     | Especifica o objetivo de minimização: _tool switches_, _makespan_ ou _flowtime_, respectivamente.       |
| **instance**      | \<path\>     | Especifica o path da instância a ser utilizada.     |
| **input**      | \<path\>     | Especifica o path do arquivo contendo o conjunto de instâncias a ser utilizado. Deve ser um .txt contendo os paths das instâncias.     |
| **iterations**  | int     | Especifica a quantidade de iterações a serem realizadas pela ILS. Default = 1000.     |
| **runs**      | int     | Especifica a quantidade de execuções a serem realizadas sob cada instância. Default = 1.     |

**Exemplo** : ./testManager --instance instancias_txt/SSP-NPM-I/ins1_m\=2_j\=10_t\=10_var\=1.txt --objective 1