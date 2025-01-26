#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AST* newASTNode(const char *name) {
    AST *node = (AST*)malloc(sizeof(AST));
    node->name = strdup(name);
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

/* printAST chama uma função recursiva para formatar */
static void printASTRec(AST *node) {
    if (!node) return;
    printf("%s (", node->name);
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
