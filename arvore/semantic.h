#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

void buildSymtab(AST *root);
void checkSemantics(AST *root);
int getSemanticErrorCount();

#endif
