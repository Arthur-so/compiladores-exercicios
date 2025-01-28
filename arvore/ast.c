#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Mantemos a versão antiga, se quiser criar nó sem linha (line=0) */
AST* newASTNode(const char *name) {
    AST *node = (AST*)malloc(sizeof(AST));
    node->name = strdup(name);
    node->line = 0; /* linha "desconhecida" */
    node->numChildren = 0;
    node->children = NULL;
    return node;
}

/* Nova função: cria nó com a 'line' desejada */
AST* newASTNodeLine(const char *name, int line) {
    AST *node = (AST*)malloc(sizeof(AST));
    node->name = strdup(name);
    node->line = line; /* <-- registra a linha */
    node->numChildren = 0;
    node->children = NULL;
    return node;
}

void addChild(AST *parent, AST *child) {
    if (!parent || !child) return;
    parent->numChildren++;
    parent->children = (AST**)realloc(
        parent->children, 
        parent->numChildren * sizeof(AST*)
    );
    parent->children[parent->numChildren - 1] = child;
}

/* Função recursiva que imprime o nome e a linha */
static void printASTRec(AST *node) {
    if (!node) return;
    /* Ex.: imprime "var_declaracao(line=5) (...)" */
    printf("%s(line=%d) (", node->name, node->line);
    for (int i = 0; i < node->numChildren; i++) {
        printASTRec(node->children[i]);
        if (i < node->numChildren - 1) {
            printf(", ");
        }
    }
    printf(")");
}

void printAST(AST *node) {
    printASTRec(node);
    printf("\n");
}
