#ifndef AST_H
#define AST_H

typedef struct AST {
    char *name;             // Nome do nó
    int line;               // Linha do código onde o nó aparece
    struct AST **children;  // Ponteiros para filhos
    int numChildren;        // Número atual de filhos
} AST;

AST* newASTNodeLine(const char *name, int line);
AST* newASTNodeText(const char *text, int line);
AST* newASTNodeTextInt(int value, int line);
void addChild(AST *parent, AST *child);
void printAST(AST *root, int level);

#endif