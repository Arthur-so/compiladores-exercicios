#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Cria um novo nó da AST com um nome */
AST* newASTNodeLine(const char *name, int line) {
    AST *node = (AST *)malloc(sizeof(AST));
    node->name = strdup(name);
    node->line = line;
    node->numChildren = 0;
    node->children = NULL;
    return node;
}

/* Cria um novo nó da AST com texto */
AST* newASTNodeText(const char *text, int line) {
    AST *node = newASTNodeLine(text, line);
    return node;
}

/* Cria um novo nó da AST com um valor inteiro */
AST* newASTNodeTextInt(int value, int line) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Num:%d", value);
    return newASTNodeLine(buf, line);
}

/* Adiciona um filho a um nó da AST */
void addChild(AST *parent, AST *child) {
    parent->children = realloc(parent->children, sizeof(AST*) * (parent->numChildren + 1));
    parent->children[parent->numChildren] = child;
    parent->numChildren++;
}

/* Imprime AST */
void printAST(AST *node, int depth) {
    if (!node) return;

    /* Indentação para representar a profundidade */
    for (int i = 0; i < depth; i++) {
        printf("\t");
    }

    /* Imprime o nome do nó */
    printf("%s", node->name);

    /* Se tiver filhos, abre parênteses e imprime recursivamente */
    if (node->numChildren > 0) {
        printf(" (\n");
        for (int i = 0; i < node->numChildren; i++) {
            printAST(node->children[i], depth + 1);
            if (i != node->numChildren - 1) {
                printf(",\n");
            }
        }
        printf("\n");
        for (int i = 0; i < depth; i++) {
            printf("\t");
        }
        printf(")");
    }
}

void freeAST(AST *node) {
    if (node == NULL) return;
    // Libera recursivamente os filhos
    for (int i = 0; i < node->numChildren; i++) {
        freeAST(node->children[i]);
    }
    // Libera o array de filhos e o nome do nó
    free(node->children);
    free(node->name);
    free(node);
}