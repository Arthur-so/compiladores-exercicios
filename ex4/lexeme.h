#include <string.h>

enum Token {
    ID, INT, CHAR, FLOAT, DOUBLE, LONG, AUTO, BREAK, CASE, CONTINUE, DEFAULT,
    DO, ENUM, EXTERN, FOR, GOTO, IF, REGISTER, RETURN, SHORT, SIGNED,
    SIZEOF, STATIC, STRUCT, SWITCH, WHILE
};

typedef struct {
    const char *word;
    enum Token token;
} KeyWord;

extern KeyWord key_words[];

typedef struct {
    int line;
    char lexeme[64];
    enum Token token;
} Classification;

enum Token classifyLexeme(char* lexeme);
