#include <string.h>

enum Token {
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
    int line;
    char lexeme[64];
    enum Token token;
} Classification;
