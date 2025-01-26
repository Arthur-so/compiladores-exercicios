#ifndef LEXEME_H
#define LEXEME_H

#include <stdio.h>

/* Estrutura de buffer para leitura */
typedef struct {
    char *data;
    int next_char_id;
    int next_char_line;
    int size;
    int new_line;
} Buffer;

/* Funções de manipulação do buffer */
Buffer* allocate_buffer(int size);
void deallocate_buffer(Buffer* buffer);
char get_next_char(FILE *fptr, Buffer* buffer);
void unget_char(Buffer* buffer);

/* Estrutura de Token (sem enum duplicada) */
typedef struct {
    int token_code;   /* código interno do lexer (ex: 110 p/ ID, etc.) */
    char lexeme[64];
    int line;
    int column;
    int error;  /* flag de erro léxico */
    int done;   /* flag de fim de arquivo */
} Token;

/* Estrutura principal do Lexer */
typedef struct {
    char current_lexeme[64];
    int  current_state;
    Buffer *buffer;
    FILE *file;
    Token *token;
} Lexer;

/* Funções do lexer */
Lexer* initialize_lexer(const char *filename, int buffer_size);
void destroy_lexer(Lexer *lexer);
Token* get_token(Lexer *lexer);

/* Auxiliares */
int get_tipo(char ch);
int classifica_lexema(char* lexeme, int estado);

/* Tabelas do autômato */
extern int Aceita[32];
extern int AdicionaAoToken[20];
extern int AdicionaAoTokenEstado[32];
extern int T[32][20];
extern int Avance[32][20];

#endif
