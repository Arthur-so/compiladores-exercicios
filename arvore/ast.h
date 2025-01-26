/* ast.h */
#ifndef AST_H
#define AST_H

typedef struct AST {
    char *name;
    int numChildren;
    struct AST **children;
} AST;

AST* newASTNode(const char *name);
void addChild(AST *parent, AST *child);
void printAST(AST *node);

#endif
