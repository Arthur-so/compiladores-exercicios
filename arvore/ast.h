#ifndef AST_H
#define AST_H

typedef struct AST {
    char *name;
    int line;                 /* <-- Campo para armazenar a linha */
    int numChildren;
    struct AST **children;
} AST;

/* Cria um nó com line=0 (se você quiser manter a versão antiga) */
AST* newASTNode(const char *name);

/* Cria um nó com uma linha especificada */
AST* newASTNodeLine(const char *name, int line);

void addChild(AST *parent, AST *child);
void printAST(AST *node);

#endif
