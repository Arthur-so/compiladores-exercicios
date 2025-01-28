/* symtab.c */
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BucketList *hashTable[HASH_SIZE];

/* Função hash simples */
static unsigned int hash(const char *key) {
    unsigned int h = 0;
    while (*key) {
        h = (h << 4) + (*key);
        unsigned int g = h & 0xF0000000;
        if (g) h ^= g >> 24;
        h &= ~g;
        key++;
    }
    return h % HASH_SIZE;
}

void initSymTab() {
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i] = NULL;
    }
}

int st_insert(const char *name, const char *scope,
              const char *tipoID, const char *tipoDado,
              int lineno)
{
    unsigned int h = hash(name);
    BucketList *l = hashTable[h];

    /* Verifica se já existe no mesmo escopo => duplicata */
    while (l != NULL) {
        if ((strcmp(l->name, name) == 0) &&
            (strcmp(l->scope, scope) == 0)) {
            return 0; /* duplicado => erro */
        }
        l = l->next;
    }

    /* Se chegou aqui, não achou duplicata => cria nova entrada */
    l = (BucketList *)malloc(sizeof(BucketList));
    l->name   = strdup(name);
    l->scope  = strdup(scope);
    l->tipoID = strdup(tipoID);
    l->tipoDado = strdup(tipoDado);

    /* Cria listinha de linhas */
    LineList *ll = (LineList *)malloc(sizeof(LineList));
    ll->lineno = lineno;
    ll->next = NULL;
    l->lines = ll;

    l->next = hashTable[h];
    hashTable[h] = l;

    return 1;
}

BucketList *st_lookup(const char *name, const char *scope) {
    unsigned int h = hash(name);
    BucketList *l = hashTable[h];
    while (l != NULL) {
        if ((strcmp(l->name, name) == 0) &&
            (strcmp(l->scope, scope) == 0))
        {
            return l;
        }
        l = l->next;
    }
    return NULL;
}

/* st_lookup_all: busca no escopo atual, se não achar => global */
BucketList *st_lookup_all(const char *name, const char *scope) {
    BucketList *b = st_lookup(name, scope);
    if (!b) {
        b = st_lookup(name, "global");
    }
    return b;
}

void st_add_lineno(const char *name, const char *scope, int lineno) {
    BucketList *b = st_lookup_all(name, scope);
    if (!b) return; /* não achou => não faz nada */
    /* Percorre até o final da lista de linhas */
    LineList *ll = b->lines;
    while (ll->next) ll = ll->next;
    LineList *newLine = (LineList *)malloc(sizeof(LineList));
    newLine->lineno = lineno;
    newLine->next = NULL;
    ll->next = newLine;
}

/* Imprime no formato pedido */
void printSymTab() {
    printf("# TABELA DE SÍMBOLOS\n");
    printf("Nome_ID;Escopo;Tipo_ID;Tipo_dado;Linha\n");
    for (int i = 0; i < HASH_SIZE; i++) {
        BucketList *l = hashTable[i];
        while (l) {
            printf("%s;%s;%s;%s;",
                   l->name, l->scope,
                   l->tipoID, l->tipoDado);
            /* imprime linhas */
            LineList *ll = l->lines;
            int first=1;
            while (ll) {
                if (!first) printf(",");
                printf("%d", ll->lineno);
                first=0;
                ll = ll->next;
            }
            printf("\n");
            l = l->next;
        }
    }
}
