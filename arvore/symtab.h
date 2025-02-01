#define HASH_SIZE 211

/* Lista de linhas onde o identificador aparece */
typedef struct LineListRec {
    int lineno;
    struct LineListRec *next;
} LineList;

/* Entrada na tabela de símbolos */
typedef struct BucketListRec {
    char *name;           // Nome do ID
    char *scope;          // Escopo
    char *tipoID;         // Tipo
    char *tipoDado;       // Tpo do dado (variavel, funcao, ...)
    LineList *lines;      // Lista de linhas onde aparece
    struct BucketListRec *next; /* encadeamento (p/ colisões) */
} BucketList;

extern BucketList *hashTable[HASH_SIZE];

/* Inicializa a tabela hash */
void initSymTab();

/* Insere um novo ID (retorna 1 se OK, 0 se duplicado) */
int st_insert(const char *name, const char *scope, const char *tipoID, const char *tipoDado, int lineno);

/* Procura no escopo exato */
BucketList *st_lookup(const char *name, const char *scope);

/* Procura no escopo exato, mas se não achar, procura no global */
BucketList *st_lookup_all(const char *name, const char *scope);

/* Adiciona uma nova linha de uso. */
void st_add_lineno(const char *name, const char *scope, int lineno);

/* Imprime a tabela */
void printSymTab();

/* DEsaloca memória */
void freeSymTab();


