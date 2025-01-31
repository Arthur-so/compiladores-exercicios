#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


static void printIndent(int level) {
    /* Exemplo: 4 espaços por nível */
    for (int i = 0; i < level; i++) {
        printf("    "); /* 4 espaços */
    }
}

static void printASTRec(AST *node, int level) {
    if (!node) return;

    printIndent(level);
    printf("%s(\n", node->name);

    for (int i = 0; i < node->numChildren; i++) {
        printASTRec(node->children[i], level + 1);

        if (i < node->numChildren - 1) {
            printf(",\n");
        } else {
            printf("\n");
        }
    }

    printIndent(level);
    printf(")");
}


/* Função pública para imprimir a AST a partir da raiz */
void printAST(AST *root) {
    printASTRec(root, 0);
    printf("\n");
}
