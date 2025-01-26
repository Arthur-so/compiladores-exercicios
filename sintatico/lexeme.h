/* lexeme.h */
#ifndef LEXEME_H
#define LEXEME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Buffer para leitura */
typedef struct {
    char* data;
    int next_char_id;
    int next_char_line;
    int size;
    int new_line;
} Buffer;

Buffer* allocate_buffer(int size);
void deallocate_buffer(Buffer* buffer);
char get_next_char(FILE *fptr, Buffer* buffer);
void unget_char(Buffer* buffer);

/* O token agora não usa enum do Bison */
typedef struct {
    int token_code;     /* código interno do lexer (ex.: 100, 101...) */
    char lexeme[64];
    int line;
    int column;
    int error;
    int done;
} Token;

/* Estrutura de controle do lexer */
typedef struct {
    char current_lexeme[64];
    int current_state;
    Buffer *buffer;
    FILE *file;
    Token *token;
} Lexer;

/* Assinaturas das funções do lexer */
Lexer* initialize_lexer(const char *filename, int buffer_size);
void destroy_lexer(Lexer *lexer);
Token* get_token(Lexer *lexer);

int get_tipo(char ch);
int classifica_lexema(char* lexeme, int estado);

/* As tabelas do autômato */
extern int Aceita[32];
extern int AdicionaAoToken[20];
extern int AdicionaAoTokenEstado[32];
extern int T[32][20];
extern int Avance[32][20];

#endif
