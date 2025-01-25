#include <string.h>
#include "funcs.h"
#include <ctype.h>
#define HASH_SIZE 11

enum TokenId {
    ID = 10,
    NUM = 11,
    GREATER = 12,
    GREATER_EQUAL = 13,
    MINOR = 14,
    MINOR_EQUAL = 15,
    EQUAL = 16,
    EQUAL_EQUAL = 17,
    DIFFERENT = 18,
    COMMENT = 19,
    PLUS = 20,
    LESS = 21,
    MULTIPLICATION = 22,
    SEMICOLON = 23,
    COMMA = 24,
    OPEN_PARENTHESIS = 25,
    CLOSE_PARENTHESIS = 26,
    OPEN_SQUARE_BRACKETS = 27,
    CLOSE_SQUARE_BRACKETS = 28,
    OPEN_CURLY_BRACKETS = 29,
    CLOSE_CURLY_BRACKETS = 30,
    SLASH = 31
};

typedef struct {
    char current_lexeme[64];
    int current_state;
    Buffer *buffer;
    HashTable *hash_table;
    FILE *file;
    Token *token;
} Lexer;

typedef struct {
    enum TokenId token_id;
    char lexeme[64];
    int line;
    int column;
} Token;

typedef struct HashNode {
    char key[10];
    int value;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* head;
} HashTable[HASH_SIZE];

unsigned int hash(const char *str);
int get_tipo(char ch);
void addToHashTable(HashTable hashTable, const char* key, int value);
void initializeHashTable(HashTable hashTable);
int findInHashTable(HashTable hashTable, const char* key);

Lexer *initialize_lexer(const char *filename, int buffer_size);
int classifica_lexema(char* lexema, int estado, HashTable hashTable);
Token* get_token(Lexer *lexer);

