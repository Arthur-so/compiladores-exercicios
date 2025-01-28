/* semantic.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "symtab.h"

/* -------------------------------------------------
   Pilha de escopos: controla qual escopo atual
   ------------------------------------------------- */
#define MAX_SCOPE_STACK 100
static char *scopeStack[MAX_SCOPE_STACK];
static int scopeTop = -1;

static void pushScope(const char *scope) {
    if (scopeTop < MAX_SCOPE_STACK - 1) {
        scopeTop++;
        scopeStack[scopeTop] = strdup(scope);
    } else {
        fprintf(stderr, "ERRO: pilha de escopos cheia!\n");
    }
}

static void popScope() {
    if (scopeTop >= 0) {
        free(scopeStack[scopeTop]);
        scopeStack[scopeTop] = NULL;
        scopeTop--;
    } else {
        fprintf(stderr, "ERRO: popScope em pilha vazia!\n");
    }
}

static const char* topScope() {
    if (scopeTop >= 0) {
        return scopeStack[scopeTop];
    }
    return "global"; 
}

/* -------------------------------------------------
   Contador de erros semânticos
   ------------------------------------------------- */
static int semanticErrors = 0;

static void semanticError(const char *msg, int line) {
    semanticErrors++;
    fprintf(stderr, 
        "ERRO SEMANTICO: %s LINHA: %d\n", 
        msg, 
        line
    );
}

/* Expor se quiser usar em outro lugar (main.c):
   int getSemanticErrorCount(); */
int getSemanticErrorCount() {
    return semanticErrors;
}

/* -------------------------------------------------
   1) buildSymtab 
      - Faz a 1ª passada na AST
      - Insere declarações na Tabela de Símbolos
      - Usa pushScope/popScope para funções
   ------------------------------------------------- */
static void buildSymtabRec(AST *node);

void buildSymtab(AST *root) {
    initSymTab();        /* zera a tabela */
    pushScope("global"); /* escopo inicial */
    /* insere built-ins */
    st_insert("input",  "global", "int",  "funcao", 0);
    st_insert("output", "global", "void", "funcao", 0);
    buildSymtabRec(root);
    popScope();          /* sai do escopo global (no final) */
}

static void buildSymtabRec(AST *node) {
    if (!node) return;

    /* Se for "fun_declaracao", muda escopo */
    if (strcmp(node->name, "fun_declaracao") == 0) {
        /* ex.: children[0] = tipo, children[1] = ID, etc. */
        AST *tipoNode = node->children[0];
        AST *idNode   = node->children[1];
        const char *tipo = tipoNode->name; /* "INT", "VOID" etc. */
        const char *funName = idNode->name;
        int line = node->line;

        /* Insere a função no escopo 'global' */
        int ok = st_insert(funName, "global", tipo, "funcao", line);
        if (!ok) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                "ID duplicado '%s' no escopo 'global'",
                funName
            );
            semanticError(msg, line);
        }
        /* pushScope para as variáveis/parâmetros da função */
        pushScope(funName);
    }
    else if (strcmp(node->name, "var_declaracao") == 0) {
        /* children[0] = "INT"/"VOID", children[1] = ID */
        const char *tipo = node->children[0]->name;
        AST *idNode      = node->children[1];
        const char *varName = idNode->name;
        int line = node->line;

        int ok = st_insert(varName, topScope(), tipo, "variavel", line);
        if (!ok) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                "ID duplicado '%s' no escopo '%s'",
                varName, topScope()
            );
            semanticError(msg, line);
        }
    }
    else if (strcmp(node->name, "var_declaracao_array") == 0) {
        /* children[0] = tipo, children[1] = ID */
        const char *tipo = node->children[0]->name;
        AST *idNode      = node->children[1];
        const char *varName = idNode->name;
        int line = node->line;

        int ok = st_insert(varName, topScope(), tipo, "vetor", line);
        if (!ok) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                "ID duplicado '%s' no escopo '%s'",
                varName, topScope()
            );
            semanticError(msg, line);
        }
    }
    else if (strcmp(node->name, "param") == 0) {
        /* children[0] = tipo, children[1] = ID */
        const char *tipo = node->children[0]->name;
        AST *idNode      = node->children[1];
        const char *paramName = idNode->name;
        int line = node->line;

        int ok = st_insert(paramName, topScope(), tipo, "variavel", line);
        if (!ok) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                "ID duplicado '%s' no escopo '%s'",
                paramName, topScope()
            );
            semanticError(msg, line);
        }
    }
    else if (strcmp(node->name, "param_array") == 0) {
        const char *tipo = node->children[0]->name;
        AST *idNode      = node->children[1];
        const char *paramName = idNode->name;
        int line = node->line;

        int ok = st_insert(paramName, topScope(), tipo, "vetor", line);
        if (!ok) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                "ID duplicado '%s' no escopo '%s'",
                paramName, topScope()
            );
            semanticError(msg, line);
        }
    }

    /* Percorre filhos */
    for (int i = 0; i < node->numChildren; i++) {
        buildSymtabRec(node->children[i]);
    }

    /* Se terminou "fun_declaracao", popScope */
    if (strcmp(node->name, "fun_declaracao") == 0) {
        popScope();
    }
}

/* -------------------------------------------------
   2) checkSemantics 
      - Faz a 2ª passada
      - Verifica uso de IDs
      - Repete pushScope/popScope para "fun_declaracao"
   ------------------------------------------------- */
static void checkSemanticsRec(AST *node);

void checkSemantics(AST *root) {
    /* Se quiser, pushScope("global") aqui também */
    /* e popScope() no final, mas iremos replicar
       exatamente a mesma lógica do buildSymtabRec. */
    checkSemanticsRec(root);
}

static void checkSemanticsRec(AST *node) {
    if (!node) return;

    /* Ao entrar em "fun_declaracao", pushScope */
    if (strcmp(node->name, "fun_declaracao") == 0) {
        AST *idNode   = node->children[1];
        const char *funName = idNode->name;

        pushScope(funName);
    }

    /* ---- Verificações ---- */
    if (strcmp(node->name, "var") == 0) {
        /* child[0] = ID */
        AST *idNode = node->children[0];
        const char *varName = idNode->name;
        int line = idNode->line;

        /* st_lookup_all => procura no topoScope(), se não achar => global */
        BucketList *b = st_lookup_all(varName, topScope());
        if (!b) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                "ID '%s' nao declarado no escopo '%s'",
                varName, topScope()
            );
            semanticError(msg, line);
        } else {
            /* se achou, registra linha de uso */
            st_add_lineno(varName, topScope(), line);
        }
    }
    else if (strcmp(node->name, "var_array") == 0) {
        /* child[0] = ID */
        AST *idNode = node->children[0];
        const char *varName = idNode->name;
        int line = idNode->line;

        BucketList *b = st_lookup_all(varName, topScope());
        if (!b) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                "ID '%s' nao declarado no escopo '%s'",
                varName, topScope()
            );
            semanticError(msg, line);
        } else {
            st_add_lineno(varName, topScope(), line);
            /* se b->tipoDado != "vetor", erro */
            if (strcmp(b->tipoDado, "vetor") != 0) {
                char msg[256];
                snprintf(msg, sizeof(msg),
                    "ID '%s' no escopo '%s' nao é vetor e esta sendo indexado",
                    varName, topScope()
                );
                semanticError(msg, line);
            }
        }
    }
    else if (strcmp(node->name, "ativacao") == 0) {
        /* child[0] = ID */
        AST *idNode = node->children[0];
        const char *funName = idNode->name;
        int line = idNode->line;

        BucketList *b = st_lookup_all(funName, topScope());
        if (!b) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                "ID '%s' nao declarado no escopo '%s'",
                funName, topScope()
            );
            semanticError(msg, line);
        } else {
            st_add_lineno(funName, topScope(), line);
            if (strcmp(b->tipoDado, "funcao") != 0) {
                char msg[256];
                snprintf(msg, sizeof(msg),
                    "ID '%s' no escopo '%s' nao é funcao e esta sendo chamado",
                    funName, topScope()
                );
                semanticError(msg, line);
            }
        }
    }
    /* ... Outras checagens possíveis (expressao_atribuicao, etc.) ... */

    /* Percorre os filhos recursivamente */
    for (int i = 0; i < node->numChildren; i++) {
        checkSemanticsRec(node->children[i]);
    }

    /* Ao sair de "fun_declaracao", popScope */
    if (strcmp(node->name, "fun_declaracao") == 0) {
        popScope();
    }
}
