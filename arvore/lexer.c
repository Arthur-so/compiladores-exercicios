#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexeme.h"


/* ---------------------------------------------------------
   1) Funções do Buffer
   --------------------------------------------------------- */
static void fill_buffer(FILE *fptr, Buffer *buffer) {
    memset(buffer->data, 0, buffer->size);
    if (fgets(buffer->data, buffer->size, fptr) != NULL) {
        buffer->next_char_id = 0;
    }
}

Buffer* allocate_buffer(int size) {
    Buffer* buf = (Buffer*)malloc(sizeof(Buffer));
    if (!buf) {
        fprintf(stderr, "Erro ao alocar Buffer.\n");
        exit(EXIT_FAILURE);
    }
    buf->data = (char*)malloc(size);
    if (!buf->data) {
        fprintf(stderr, "Erro ao alocar data do Buffer.\n");
        free(buf);
        exit(EXIT_FAILURE);
    }
    memset(buf->data, 0, size);
    buf->next_char_id = 0;
    buf->next_char_line = 1;
    buf->size = size;
    buf->new_line = 0;
    return buf;
}

void deallocate_buffer(Buffer* buffer) {
    if (buffer) {
        free(buffer->data);
        free(buffer);
    }
}

char get_next_char(FILE *fptr, Buffer* buffer) {
    if (buffer->next_char_id >= buffer->size || buffer->data[buffer->next_char_id] == '\0') {
        fill_buffer(fptr, buffer);
        if (buffer->data[0] == '\0') {
            return EOF;
        }
        if (buffer->new_line) {
            buffer->next_char_line++;
            buffer->new_line = 0;
        }
    }
    if (buffer->data[buffer->next_char_id] == '\n') {
        buffer->new_line = 1;
    }
    return buffer->data[buffer->next_char_id];
}

void unget_char(Buffer* buffer) {
    if (buffer->next_char_id > 0) {
        buffer->next_char_id--;
    }
}

/* ---------------------------------------------------------
   2) Inicialização e destruição do Lexer
   --------------------------------------------------------- */
Lexer* initialize_lexer(const char *filename, int buffer_size) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Erro ao abrir arquivo");
        return NULL;
    }
    Buffer *b = allocate_buffer(buffer_size);
    Token *t  = (Token*)malloc(sizeof(Token));

    Lexer *lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->buffer = b;
    lexer->file = f;
    lexer->token = t;
    lexer->current_state = 0;
    lexer->current_lexeme[0] = '\0';

    t->error = 0;
    t->done = 0;
    return lexer;
}

void destroy_lexer(Lexer *lexer) {
    if (!lexer) return;
    deallocate_buffer(lexer->buffer);
    free(lexer->token);
    fclose(lexer->file);
    free(lexer);
}

/* ---------------------------------------------------------
   3) get_tipo() -> classifica caractere em índice do T[][] 
   --------------------------------------------------------- */
int get_tipo(char ch) {
    if (isalpha(ch)) {
        return 0; // letra
    }
    else if (isdigit(ch)) {
        return 1; // dígito
    }
    else {
        switch(ch) {
            case '+': return 2;
            case '-': return 3;
            case '*': return 4;
            case ';': return 5;
            case ',': return 6;
            case '(': return 7;
            case ')': return 8;
            case '[': return 9;
            case ']': return 10;
            case '{': return 11;
            case '}': return 12;
            case '>': return 13;
            case '<': return 14;
            case '=': return 15;
            case '!': return 16;
            case '/': return 17;
            default:
                if (isspace(ch) || ch == EOF) {
                    return 18;
                } else {
                    return 19; // desconhecido
                }
        }
    }
}

/* ---------------------------------------------------------
   4) classifica_lexema() -> dado estado final e lexema,
      retorna nosso código interno (L_...)
   --------------------------------------------------------- */
int classifica_lexema(char* lexeme, int estado) {
    int code = estado; // de início, use o "estado" numérico

    /* Ex: estado=10 => ID, 11 => NUM, etc. */
    if (estado == 10) { // ID
        if (strcmp(lexeme,"else")==0)    code = L_KW_ELSE;
        else if (strcmp(lexeme,"if")==0) code = L_KW_IF;
        else if (strcmp(lexeme,"int")==0) code = L_KW_INT;
        else if (strcmp(lexeme,"return")==0) code = L_KW_RETURN;
        else if (strcmp(lexeme,"void")==0) code = L_KW_VOID;
        else if (strcmp(lexeme,"while")==0) code = L_KW_WHILE;
    }

    return code;
}

/* ---------------------------------------------------------
   5) get_token() -> loop do autômato para pegar próximo token
   --------------------------------------------------------- */
Token* get_token(Lexer *lexer) {
    char c = 0;
    int char_lexeme_id = 0;
    char aux_lexeme[64];
    int char_code = 0;
    int novo_estado = 0;
    int avance = 0;

    lexer->token->error = 0;
    lexer->token->done = 0;
    memset(aux_lexeme, 0, sizeof(aux_lexeme));

    while (1) {
        /* Se c == EOF => fim de arquivo */
        if (c == EOF) {
            lexer->token->done = 1;
            return lexer->token;
        }

        /* ---------- TRATAMENTO DE ERRO LÉXICO ---------- */
        if (lexer->current_state == -1) {
            /* Guardamos o char que causou o erro */
            aux_lexeme[char_lexeme_id++] = c;
            aux_lexeme[char_lexeme_id] = '\0';
            lexer->buffer->next_char_id++;

            /* Marcamos erro e preenchemos token */
            lexer->token->error = 1;
            strncpy(lexer->token->lexeme, aux_lexeme, sizeof(lexer->token->lexeme));
            lexer->token->line = lexer->buffer->next_char_line;
            lexer->token->column = lexer->buffer->next_char_id;

            /* Em vez de break, RETORNAMOS O TOKEN AGORA! */
            return lexer->token;
        }

        /* ---------- VERIFICA SE ESTADO É DE ACEITAÇÃO ---------- */
        if (Aceita[lexer->current_state]) {
            aux_lexeme[char_lexeme_id] = '\0';

            /* classifica_lexema decide se é ID, NUM, etc. */
            lexer->token->token_code = classifica_lexema(aux_lexeme, lexer->current_state);
            lexer->token->line   = lexer->buffer->next_char_line;
            lexer->token->column = lexer->buffer->next_char_id;
            strncpy(lexer->token->lexeme, aux_lexeme, sizeof(lexer->token->lexeme));

            lexer->current_state = 0;
            return lexer->token;
        }

        /* Lê próximo caractere */
        c = get_next_char(lexer->file, lexer->buffer);
        if (c == EOF) {
            lexer->token->done = 1;
            return lexer->token;
        }

        /* Consulta tabela de transição */
        char_code = get_tipo(c);
        novo_estado = T[lexer->current_state][char_code];
        avance = Avance[lexer->current_state][char_code];

        /* Se for pra avançar, adiciona char no aux_lexeme se apropriado */
        if (avance) {
            if (AdicionaAoToken[char_code] &&
                (AdicionaAoTokenEstado[novo_estado] || 
                 AdicionaAoTokenEstado[lexer->current_state])) 
            {
                aux_lexeme[char_lexeme_id++] = c;
            }
            lexer->buffer->next_char_id++;
        }

        lexer->current_state = novo_estado;
    }

    /* Se chegou aqui de forma não usual, retorne token */
    return lexer->token;
}

/* ---------------------------------------------------------
   6) Tabelas do autômato
   --------------------------------------------------------- */
int Aceita[32] = {
  0,0,0,0,0,0,0,0,0,0,
  1,1,
  1,1,1,1,1,1,1,
  1,
  1,1,1,1,1,1,1,1,1,1,1,1
};

int AdicionaAoToken[20] = {
  1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,0,0
};

int AdicionaAoTokenEstado[32] = {
  1,1,1,1,1,1,1,1,0,0,
  1,1,
  1,1,1,1,1,1,1,
  1,
  1,1,1,1,1,1,1,1,1,1,1,1
};


/* ------------------------------------------------------------------------- *
 *  Tabela de Transição T[32][20]
 *  As linhas (estados) vão de 0 a 31
 *  Estados 10..31 são finais (L_...), retornam diretamente o token
 * ------------------------------------------------------------------------- */

int T[32][20] = {
    /*                      Colunas:
       0   1      2        3        4        5         6         7         8         9
       L   D      +        -        *        ;         ,         (         )         [
      
      10  11     12       13       14       15        16        17        18       19
      ]   {      }        >        <        =         !         /         B        outro
    */

    /*------------------------------------------------------------------------------------*
     * Estado 0
     *------------------------------------------------------------------------------------*/
    {  1,  2,  L_PLUS,   L_MINUS,  L_MULT,   L_SEMICOLON, L_COMMA, L_OPAREN, L_CPAREN, L_OBRACK,
       L_CBRACK, L_OCURLY, L_CCURLY, 3,        4,           5,        6,       7,        0,        -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 1 (letra) => formando um ID
     *------------------------------------------------------------------------------------*/
    {  1, -1,  L_ID,     L_ID,     L_ID,     L_ID,         L_ID,     L_ID,    L_ID,     L_ID,
       L_ID,   L_ID,     L_ID,     L_ID,     L_ID,         L_ID,     L_ID,    L_ID,     L_ID,     -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 2 (dígito) => formando um NUM
     *------------------------------------------------------------------------------------*/
    { -1,  2,  L_NUM,    L_NUM,    L_NUM,    L_NUM,        L_NUM,    L_NUM,   L_NUM,    L_NUM,
       L_NUM,  L_NUM,    L_NUM,    L_NUM,    L_NUM,        L_NUM,    L_NUM,   L_NUM,    L_NUM,    -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 3 (>x)
     *------------------------------------------------------------------------------------*/
    { L_GREATER, L_GREATER, L_GREATER,  L_GREATER,  L_GREATER,  L_GREATER, 
      L_GREATER, L_GREATER, L_GREATER,  L_GREATER,  L_GREATER,  L_GREATER,
      L_GREATER, L_GREATER, L_GREATER,  L_GREATER_EQ,
      L_GREATER, L_GREATER, L_GREATER,  -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 4 (<x)
     *------------------------------------------------------------------------------------*/
    { L_MINOR, L_MINOR, L_MINOR, L_MINOR, L_MINOR, L_MINOR, L_MINOR, L_MINOR,
      L_MINOR, L_MINOR, L_MINOR, L_MINOR, L_MINOR, L_MINOR, L_MINOR, L_MINOR_EQ,
      L_MINOR, L_MINOR, L_MINOR, -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 5 (=x)
     *------------------------------------------------------------------------------------*/
    { L_EQUAL, L_EQUAL, L_EQUAL, L_EQUAL, L_EQUAL, L_EQUAL, L_EQUAL, L_EQUAL,
      L_EQUAL, L_EQUAL, L_EQUAL, L_EQUAL, L_EQUAL, L_EQUAL, L_EQUAL, L_EQUAL_EQ,
      L_EQUAL, L_EQUAL, L_EQUAL, -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 6 (!x)
     *------------------------------------------------------------------------------------*/
    { -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, L_DIFF,
      -1, -1, -1, -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 7 (/x)
     *------------------------------------------------------------------------------------*/
    { L_SLASH, L_SLASH, L_SLASH, L_SLASH,  8,       L_SLASH, L_SLASH, L_SLASH,
      L_SLASH, L_SLASH, L_SLASH, L_SLASH, L_SLASH, L_SLASH, L_SLASH, L_SLASH,
      L_SLASH, L_SLASH, L_SLASH, -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 8 (/*xxx)
     *------------------------------------------------------------------------------------*/
    { 8, 8, 8, 8, 9, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8 },

    /*------------------------------------------------------------------------------------*
     * Estado 9 (/*xxx*xxx)
     *------------------------------------------------------------------------------------*/
    { 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8,
      8, L_COMMENT, 8, 8 },

    /*------------------------------------------------------------------------------------*
     * A partir de 10..31 são estados finais => 0 em todas as colunas
     *------------------------------------------------------------------------------------*/

    /* Estado 10: L_ID */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 11: L_NUM */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 12: L_GREATER */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 13: L_GREATER_EQ */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 14: L_MINOR */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 15: L_MINOR_EQ */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 16: L_EQUAL */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 17: L_EQUAL_EQ */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 18: L_DIFF */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 19: L_COMMENT */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 20: L_PLUS */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 21: L_MINUS */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 22: L_MULT */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 23: L_SEMICOLON */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 24: L_COMMA */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 25: L_OPAREN */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 26: L_CPAREN */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 27: L_OBRACK */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 28: L_CBRACK */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 29: L_OCURLY */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 30: L_CCURLY */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 31: L_SLASH */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

};

int Avance[32][20] = {
   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
   /* states 10..31 => todos 0 */
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
