#include "lexeme.h"

KeyWord key_words[] = {
    {"int", INT}, {"char", CHAR}, {"float", FLOAT}, {"double", DOUBLE},
    {"long", LONG}, {"auto", AUTO}, {"break", BREAK}, {"case", CASE},
    {"continue", CONTINUE}, {"default", DEFAULT}, {"do", DO},
    {"enum", ENUM}, {"extern", EXTERN}, {"for", FOR}, {"goto", GOTO},
    {"if", IF}, {"register", REGISTER}, {"return", RETURN},
    {"short", SHORT}, {"signed", SIGNED}, {"sizeof", SIZEOF},
    {"static", STATIC}, {"struct", STRUCT}, {"switch", SWITCH},
    {"while", WHILE},
};

int is_char(char c) {
    int ascii_code = (int)c;
    if ((ascii_code >= 97 && ascii_code <= 122) || (ascii_code >= 65 && ascii_code <= 90))
        return 1;
    return 0;
}

int is_num(char c) {
    int ascii_code = (int)c;
    if ((ascii_code >= 48 && ascii_code <= 57))
        return 1;
    return 0;
}

enum Token classifyLexeme(char* lexeme) {
    int total_words = sizeof(key_words) / sizeof(KeyWord);
    for (int i = 0; i < total_words; i++) {
        if (strcmp(lexeme, key_words[i].word) == 0) {
            return key_words[i].token; // Palavra-chave encontrada
        }
    }
    return ID;
}