/* symtab.h */
#ifndef SYMTAB_H
#define SYMTAB_H

#define HASH_SIZE 211

/* Lista de linhas onde o identificador aparece */
typedef struct LineListRec {
    int lineno;
    struct LineListRec *next;
} LineList;

/* Estrutura de cada "entrada" na tabela de símbolos */
typedef struct BucketListRec {
    char *name;           /* Nome do ID */
    char *scope;          /* ex.: "global", "fun_main", etc. */
    char *tipoID;         /* ex.: "int", "void" */
    char *tipoDado;       /* "variavel", "vetor", "funcao" */
    LineList *lines;      /* linhas onde aparece (decl/uso) */

    struct BucketListRec *next; /* encadeamento (colisões) */
} BucketList;

extern BucketList *hashTable[HASH_SIZE];

/* Inicializa a hashTable */
void initSymTab();

/* Insere um novo ID (retorna 1 se OK, 0 se duplicado) */
int st_insert(const char *name, const char *scope,
              const char *tipoID, const char *tipoDado,
              int lineno);

/* Procura no escopo exato */
BucketList *st_lookup(const char *name, const char *scope);

/* Similar, mas se não achar no escopo, procura no global
   (ajuste se quiser algo mais complexo) */
BucketList *st_lookup_all(const char *name, const char *scope);

/* Adiciona uma nova linha de uso. */
void st_add_lineno(const char *name, const char *scope, int lineno);

/* Imprime a Tabela no formato pedido. */
void printSymTab();

#endif
