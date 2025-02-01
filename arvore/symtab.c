#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BucketList *hashTable[HASH_SIZE];

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

int st_insert(const char *name, const char *scope, const char *tipoID, const char *tipoDado, int lineno) {
    unsigned int h = hash(name);
    BucketList *bucket_list = hashTable[h];

    /* Verifica se já existe no mesmo escopo, caso exista: duplicado (erro) */
    while (bucket_list != NULL) {
        if ((strcmp(bucket_list->name, name) == 0) &&
            (strcmp(bucket_list->scope, scope) == 0)) {
            return 0; // Erro
        }
        bucket_list = bucket_list->next;
    }

    /* Cria nova entrada */
    bucket_list = (BucketList *)malloc(sizeof(BucketList));
    bucket_list->name   = strdup(name);
    bucket_list->scope  = strdup(scope);
    bucket_list->tipoID = strdup(tipoID);
    bucket_list->tipoDado = strdup(tipoDado);

    /* Cria lista de linhas */
    LineList *line_list = (LineList *)malloc(sizeof(LineList));
    line_list->lineno = lineno;
    line_list->next = NULL;
    bucket_list->lines = line_list;

    bucket_list->next = hashTable[h];
    hashTable[h] = bucket_list;

    return 1;
}

BucketList *st_lookup(const char *name, const char *scope) {
    unsigned int h = hash(name);
    BucketList *bucket_list = hashTable[h];

    while (bucket_list != NULL) {
        if ((strcmp(bucket_list->name, name) == 0) && (strcmp(bucket_list->scope, scope) == 0)) {
            return bucket_list;
        }

        bucket_list = bucket_list->next;
    }
    /* Não encontrou no escopo atual */
    return NULL;
}

/* st_lookup_all: busca no escopo atual, se não achar busca no global */
BucketList *st_lookup_all(const char *name, const char *scope) {
    BucketList *bucket_list = st_lookup(name, scope);

    if (!bucket_list) {
        bucket_list = st_lookup(name, "global");
    }

    return bucket_list;
}

void st_add_lineno(const char *name, const char *scope, int lineno) {
    BucketList *bucket_list = st_lookup_all(name, scope);
    /* Se não achou, não faz nada */
    if (!bucket_list) return;

    /* Percorre até o final da lista de linhas */
    LineList *line_list = bucket_list->lines;
    while (line_list->next) 
        line_list = line_list->next;
    
    /* Adiciona na lista */
    LineList *new_line = (LineList *)malloc(sizeof(LineList));
    new_line->lineno = lineno;
    new_line->next = NULL;
    line_list->next = new_line;
}

/* Imprime tabela */
void printSymTab() {
    printf("# TABELA DE SÍMBOLOS\n");
    printf("Nome_ID;Escopo;Tipo_ID;Tipo_dado;Linha\n");

    for (int i = 0; i < HASH_SIZE; i++) {
        BucketList *bucket_list = hashTable[i];

        while (bucket_list) {
            printf("%s;%s;%s;%s;", bucket_list->name, bucket_list->scope, bucket_list->tipoID, bucket_list->tipoDado);
            
            /* Imprime linhas */
            LineList *line_list = bucket_list->lines;
            int first=1;
            while (line_list) {
                if (!first) 
                    printf(",");
                
                printf("%d", line_list->lineno);
                
                first=0;
                line_list = line_list->next;
            }
            printf("\n");
            bucket_list = bucket_list->next;
        }
    }
}

void freeSymTab() {
    for (int i = 0; i < HASH_SIZE; i++) {
        BucketList *l = hashTable[i];
        while (l) {
            BucketList *temp = l;
            l = l->next;
            // Libera a lista de linhas
            LineList *ll = temp->lines;
            while (ll) {
                LineList *tmpLL = ll;
                ll = ll->next;
                free(tmpLL);
            }
            free(temp->name);
            free(temp->scope);
            free(temp->tipoID);
            free(temp->tipoDado);
            free(temp);
        }
        hashTable[i] = NULL;
    }
}