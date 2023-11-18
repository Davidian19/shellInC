# shellInC

Este é um programa de shell simples implementado em linguagem C. Ele oferece funcionalidades básicas de um interpretador de comandos, permitindo a execução de comandos simples, encadeamento de comandos com pipes, redirecionamento de saída para arquivos e execução em segundo plano.

##Funcionalidades Principais
Execução de Comandos Simples: O shell permite a execução de comandos simples, como aqueles digitados diretamente na linha de comando.

Encadeamento de Comandos com Pipes: É possível encadear dois comandos usando o operador de pipe (|). O resultado da execução do primeiro comando é passado como entrada para o segundo.

Redirecionamento de Saída para Arquivos: O shell suporta redirecionamento de saída para arquivos, tanto para criação quanto para anexação. Os operadores > e >> são utilizados para esse propósito.

Execução em Segundo Plano: Comandos podem ser executados em segundo plano, permitindo que o shell continue aceitando novos comandos enquanto o comando em segundo plano é executado.

Histórico de Comandos: O shell mantém o histórico do último comando executado, permitindo a repetição do comando anterior usando !!.

Modo de Lote (Batch Mode): O programa pode ser executado em modo de lote, onde os comandos são lidos de um arquivo de lote e executados sequencialmente.

##Compilação

Para compilar o programa, utilize o Makefile fornecido. O comando make criará um executável chamado shell.
`make`

##Execução

Para iniciar o shell interativo, execute o seguinte comando:
`./shell`

Para executar comandos a partir de um arquivo de lote, forneça o caminho do arquivo como argumento:
`./shell arquivo_de_lote.txt`


##Comandos Especiais
style parallel: Alterna o shell para o modo paralelo, onde os comandos são executados em paralelo.
style sequential: Alterna o shell para o modo sequencial, onde os comandos são executados sequencialmente.

##Comandos Adicionais
exit: Encerra o shell.
!!: Repete o último comando executado.

Limpeza

Para limpar os arquivos objeto e o executável, utilize o seguinte comando:
`make clean`
Este projeto foi desenvolvido como parte de um exercício para entender os conceitos básicos de um interpretador de comandos em sistemas Unix-like. Sinta-se à vontade para explorar, modificar e melhorar conforme necessário.
