#include <string.h>
#include "funcs.h"
#include <ctype.h>
#define NUM_KEYWORDS 6

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

extern const char* keywords[NUM_KEYWORDS];
extern int keyword_ids[NUM_KEYWORDS];

typedef struct {
    enum TokenId token_id;
    char lexeme[64];
    int line;
    int column;
    int error;
    int done;
} Token;

typedef struct {
    char current_lexeme[64];
    int current_state;
    Buffer *buffer;
    FILE *file;
    Token *token;
} Lexer;


int get_tipo(char ch);

Lexer *initialize_lexer(const char *filename, int buffer_size);
int classifica_lexema(char* lexema, int estado);
Token* get_token(Lexer *lexer);

extern int Aceita[32];
extern int AdicionaAoToken[20];
extern int AdicionaAoTokenEstado[32];
extern int T[32][20];
extern int Avance[32][20];