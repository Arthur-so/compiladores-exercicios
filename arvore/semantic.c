#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "symtab.h"

/* Pilha de escopos: controla qual escopo atual */
#define MAX_SCOPE_STACK 100
static char *scopeStack[MAX_SCOPE_STACK];
static int scopeTop = -1;

/* Adiciona novo escopo */
static void pushScope(const char *scope) {
    if (scopeTop < MAX_SCOPE_STACK - 1) {
        scopeTop++;
        scopeStack[scopeTop] = strdup(scope);
    }
    else {
        fprintf(stderr, "ERRO: pilha de escopos cheia!\n");
    }
}

/* Remove escopo atual */
static void popScope() {
    if (scopeTop >= 0) {
        free(scopeStack[scopeTop]);
        scopeStack[scopeTop] = NULL;
        scopeTop--;
    }
    else {
        fprintf(stderr, "ERRO: popScope em pilha vazia!\n");
    }
}

/* Retorna escopo atual */
static const char* topScope() {
    if (scopeTop >= 0) {
        return scopeStack[scopeTop];
    }
    return "global"; 
}

/* Contador de erros */
static int semanticErrors = 0;

static void semanticError(const char *msg, int line) {
    semanticErrors++;
    fprintf(stderr, 
        "ERRO SEMANTICO: %s LINHA: %d\n", 
        msg, 
        line
    );
}


static void buildSymtabRec(AST *node);

/* Faz a 1ª passada na AST e insere declarações na Tabela de Símbolos */
void buildSymtab(AST *root) {
    initSymTab();        /* zera a tabela */
    pushScope("global"); /* escopo inicial */
    /* insere built-ins */
    st_insert("input",  "global", "int",  "funcao", 0);
    st_insert("output", "global", "void", "funcao", 0);
    buildSymtabRec(root);
    popScope();          /* sai do escopo global */
}

/* Percorre a AST */
static void buildSymtabRec(AST *node) {
    if (!node)
        return;

    /* Se for "Func", muda escopo */
    if (strcmp(node->name, "Func") == 0) {
        /* children[0] = tipo_especificador, children[1] = ID, children[2] = Param_list, children[3] = Body */
        AST *tipoNode = node->children[0];
        AST *idNode   = node->children[1];
        const char *tipo = tipoNode->name; /* "INT", "VOID" */
        const char *funName = idNode->name;
        int line = node->line;

        /* Insere a função no escopo 'global' */
        int ok = st_insert(funName, "global", tipo, "funcao", line);
        if (!ok) {
            char msg[256];
            snprintf(msg, sizeof(msg), "ID duplicado '%s' no escopo 'global'", funName);
            semanticError(msg, line);
        }
        pushScope(funName);
    }
    else if (strcmp(node->name, "Decl_var") == 0) {
        /* children[0] = tipo_especificador, children[1] = ID */
        const char *tipo = node->children[0]->name;
        AST *idNode      = node->children[1];
        const char *varName = idNode->name;
        int line = node->line;

        int ok = st_insert(varName, topScope(), tipo, "variavel", line);
        if (!ok) {
            char msg[256];
            snprintf(msg, sizeof(msg), "ID duplicado '%s' no escopo '%s'", varName, topScope());
            semanticError(msg, line);
        }
    }
    else if (strcmp(node->name, "Decl_var_array") == 0) {
        /* children[0] = tipo_especificador, children[1] = ID, children[2] = tamanho */
        const char *tipo = node->children[0]->name;
        AST *idNode = node->children[1];
        const char *varName = idNode->name;
        int line = node->line;

        int ok = st_insert(varName, topScope(), tipo, "vetor", line);
        if (!ok) {
            char msg[256];
            snprintf(msg, sizeof(msg), "ID duplicado '%s' no escopo '%s'", varName, topScope());
            semanticError(msg, line);
        }
    }
    else if (strcmp(node->name, "Param") == 0) {
        /* children[0] = tipo_especificador, children[1] = ID */
        const char *tipo = node->children[0]->name;
        AST *idNode      = node->children[1];
        const char *paramName = idNode->name;
        int line = node->line;

        int ok = st_insert(paramName, topScope(), tipo, "variavel", line);
        if (!ok) {
            char msg[256];
            snprintf(msg, sizeof(msg), "ID duplicado '%s' no escopo '%s'", paramName, topScope());
            semanticError(msg, line);
        }
    }
    else if (strcmp(node->name, "Param_array") == 0) {
        /* children[0] = tipo_especificador, children[1] = ID */
        const char *tipo = node->children[0]->name;
        AST *idNode      = node->children[1];
        const char *paramName = idNode->name;
        int line = node->line;

        /* Insere o parâmetro array na tabela de símbolos */
        int ok = st_insert(paramName, topScope(), tipo, "vetor", line);
        if (!ok) {
            char msg[256];
            snprintf(msg, sizeof(msg), "ID duplicado '%s' no escopo '%s'", paramName, topScope());
            semanticError(msg, line);
        }
    }
    else if (strcmp(node->name, "Param_list") == 0) {
        /* Percorre todos os parâmetros na lista */
        for (int i = 0; i < node->numChildren; i++) {
            buildSymtabRec(node->children[i]);
        }
        return; // Evita processar novamente os filhos
    }
    else if (strcmp(node->name, "Param_list_void") == 0) {
        /* Não há parâmetros */
        return;
    }

    /* Percorre os filhos */
    for (int i = 0; i < node->numChildren; i++) {
        buildSymtabRec(node->children[i]);
    }

    /* Se terminou "Func", popScope */
    if (strcmp(node->name, "Func") == 0) {
        popScope();
    }
}

static void checkSemanticsRec(AST *node);

/* Faz a 2ª passada para verificar uso de IDs e repete pushScope/popScope para "Func" */
void checkSemantics(AST *root) {
    checkSemanticsRec(root);
}

static void checkSemanticsRec(AST *node) {
    if (!node) return;

    /* Ao entrar em "Func", pushScope */
    if (strcmp(node->name, "Func") == 0) {
        AST *idNode   = node->children[1];
        const char *funName = idNode->name;

        pushScope(funName);
    }
    else if (strcmp(node->name, "Param_list") == 0) {
        /* Processa cada parâmetro na lista */
        for (int i = 0; i < node->numChildren; i++) {
            checkSemanticsRec(node->children[i]);
        }
        return; // Evita processar novamente os filhos
    }
    else if (strcmp(node->name, "Param_list_void") == 0) {
        /* Nada p/ fazer */
        return;
    }

    if (strcmp(node->name, "Var") == 0) {
        /* children[0] = ID */
        AST *idNode = node->children[0];
        const char *varName = idNode->name;
        int line = node->line;

        /* st_lookup_all: procura no topoScope(), se não achar é global */
        BucketList *b = st_lookup_all(varName, topScope());
        if (!b) {
            char msg[256];
            snprintf(msg, sizeof(msg), "ID '%s' nao declarado no escopo '%s'", varName, topScope());
            semanticError(msg, line);
        } else {
            /* se achou, registra linha de uso */
            st_add_lineno(varName, topScope(), line);
        }
    }
    else if (strcmp(node->name, "Var_array") == 0) {
        /* children[0] = ID */
        AST *idNode = node->children[0];
        const char *varName = idNode->name;
        int line = node->line;

        BucketList *b = st_lookup_all(varName, topScope());
        if (!b) {
            char msg[256];
            snprintf(msg, sizeof(msg), "ID '%s' nao declarado no escopo '%s'", varName, topScope());
            semanticError(msg, line);
        } else {
            st_add_lineno(varName, topScope(), line);
            /* se b->tipoDado != "vetor", então erro */
            if (strcmp(b->tipoDado, "vetor") != 0) {
                char msg[256];
                snprintf(msg, sizeof(msg), "ID '%s' no escopo '%s' nao é vetor e esta sendo indexado", varName, topScope());
                semanticError(msg, line);
            }
        }
    }
    else if (strcmp(node->name, "Function_call") == 0) {
        /* children[0] = ID (nome da função) */
        AST *idNode = node->children[0];
        const char *funName = idNode->name;
        int line = node->line;

        BucketList *b = st_lookup_all(funName, topScope());
        if (!b) {
            char msg[256];
            snprintf(msg, sizeof(msg), "ID '%s' nao declarado no escopo '%s'", funName, topScope());
            semanticError(msg, line);
        } else {
            st_add_lineno(funName, topScope(), line);
            if (strcmp(b->tipoDado, "funcao") != 0) {
                char msg[256];
                snprintf(msg, sizeof(msg), "ID '%s' no escopo '%s' nao é funcao e esta sendo chamado", funName, topScope());
                semanticError(msg, line);
            }
        }
    }

    /* Percorre os filhos recursivamente */
    for (int i = 0; i < node->numChildren; i++) {
        checkSemanticsRec(node->children[i]);
    }

    /* Ao sair de "Func", popScope */
    if (strcmp(node->name, "Func") == 0) {
        popScope();
    }
}
