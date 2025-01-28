#ifndef LEXEME_H
#define LEXEME_H

#include <stdio.h>
/* ---------------------------
   Códigos internos do lexer
   --------------------------- */
#define L_KW_ELSE      100
#define L_KW_IF        101
#define L_KW_INT       102
#define L_KW_RETURN    103
#define L_KW_VOID      104
#define L_KW_WHILE     105

#define L_ID           10
#define L_NUM          11
#define L_GREATER      12
#define L_GREATER_EQ   13
#define L_MINOR        14
#define L_MINOR_EQ     15
#define L_EQUAL        16
#define L_EQUAL_EQ     17
#define L_DIFF         18
#define L_COMMENT      19
#define L_PLUS         20
#define L_MINUS        21
#define L_MULT         22
#define L_SEMICOLON    23
#define L_COMMA        24
#define L_OPAREN       25
#define L_CPAREN       26
#define L_OBRACK       27
#define L_CBRACK       28
#define L_OCURLY       29
#define L_CCURLY       30
#define L_SLASH        31

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
