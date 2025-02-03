# Compilador - Analisador Léxico, Sintático e Semântico

Este é um projeto de um **compilador** desenvolvido para realizar a **análise léxica, sintática e semântica** de programas escritos em uma linguagem específica. O compilador utiliza uma **tabela hash** para otimizar a identificação de palavras-chave e tokens, melhorando a eficiência da análise.

## Como Compilar
Para compilar o projeto, utilize o comando:
```sh
make
```
Isso irá gerar o executável **`compilador`**.

## Como Executar
O compilador aceita diferentes comandos para exibir os resultados das análises:

### **Análise Léxica** (Tokens)
```sh
./compilador <nome_do_arquivo> -L
```
**Exemplo:**
```sh
./compilador teste1.c -L
```

**Descrição:**
- Este comando **exibe os tokens identificados** no código-fonte junto com seus respectivos lexemas e números de linha.

### **Exibir a Árvore Sintática (AST)**
```sh
./compilador <nome_do_arquivo> -P
```
**Exemplo:**
```sh
./compilador teste1.c -P
```

**Descrição:**
- Gera e imprime a **Árvore Sintática Abstrata (AST)** do programa analisado.

### **Exibir a Tabela de Símbolos**
```sh
./compilador <nome_do_arquivo> -S
```
**Exemplo:**
```sh
./compilador teste1.c -S
```

**Descrição:**
- Mostra a **tabela de símbolos** com os identificadores declarados, seus escopos e linhas onde aparecem.

## Dependências
Para rodar este compilador, é necessário ter:
- **GCC** instalado.
- **Make** para compilação automática.


