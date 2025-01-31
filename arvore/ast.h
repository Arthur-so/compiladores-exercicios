#ifndef AST_H
#define AST_H

typedef struct AST {
    char *name;             // Nome do nó
    int line;             // Linha do código onde o nó aparece
    struct AST **children;  // Array de ponteiros para filhos
    int numChildren;        // Número atual de filhos
    int capacity;           // Capacidade do array de filhos
} AST;

/* Funções para criação e manipulação de nós da AST */
AST* newASTNodeLine(const char *name, int line);
AST* newASTNodeText(const char *text, int line);
AST* newASTNodeTextInt(int value, int line);
void addChild(AST *parent, AST *child);
void printAST(AST *root, int level); // Função para imprimir a AST

#endif
