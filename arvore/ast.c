#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Função para criar um novo nó da AST com nome e linha */
AST* newASTNodeLine(const char *name, int line) {
    AST *node = (AST*)malloc(sizeof(AST));
    node->name = strdup(name);
    node->line = line;
    node->numChildren = 0;
    node->capacity = 4; // Capacidade inicial
    node->children = (AST**)malloc(sizeof(AST*) * node->capacity);
    return node;
}

/* Função para criar um nó da AST com texto */
AST* newASTNodeText(const char *text, int line) {
    AST *node = newASTNodeLine("text", line);
    free(node->name);
    node->name = strdup(text);
    return node;
}

/* Função para criar um nó da AST com valor inteiro */
AST* newASTNodeTextInt(int value, int line) {
    char buf[32];
    sprintf(buf, "NUM:%d", value);
    return newASTNodeText(buf, line);
}

/* Função para adicionar um filho a um nó da AST */
void addChild(AST *parent, AST *child) {
    if (child == NULL) return; // Evita adicionar nós nulos
    if (parent->numChildren >= parent->capacity) {
        parent->capacity *= 2;
        parent->children = (AST**)realloc(parent->children, sizeof(AST*) * parent->capacity);
    }
    parent->children[parent->numChildren++] = child;
}

/* Função auxiliar para imprimir indentação */
void printIndent(int level) {
    for (int i = 0; i < level; i++) {
        printf("    "); // 4 espaços por nível
    }
}

/* Função para imprimir a AST */
void printAST(AST *root, int level) {
    if (!root) return;

    printIndent(level);
    printf("%s", root->name);

    if (root->numChildren > 0) {
        printf(" (\n");
        for (int i = 0; i < root->numChildren; i++) {
            printAST(root->children[i], level + 1);
            if (i < root->numChildren - 1) {
                printf(",\n");
            } else {
                printf("\n");
            }
        }
        printIndent(level);
        printf(")");
    }
}
