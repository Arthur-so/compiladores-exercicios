#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexeme.h"

/* ---------------------------
   Códigos internos do lexer
   --------------------------- */
#define L_KW_ELSE      100
#define L_KW_IF        101
#define L_KW_INT       102
#define L_KW_RETURN    103
#define L_KW_VOID      104
#define L_KW_WHILE     105
#define L_ID           110
#define L_NUM          111
#define L_GREATER      112
#define L_GREATER_EQ   113
#define L_MINOR        114
#define L_MINOR_EQ     115
#define L_EQUAL        116
#define L_EQUAL_EQ     117
#define L_DIFF         118
#define L_COMMENT      119
#define L_PLUS         120
#define L_MINUS        121
#define L_MULT         122
#define L_SEMICOLON    123
#define L_COMMA        124
#define L_OPAREN       125
#define L_CPAREN       126
#define L_OBRACK       127
#define L_CBRACK       128
#define L_OCURLY       129
#define L_CCURLY       130
#define L_SLASH        131

#define ST_ID          10
#define ST_NUM         11
#define ST_GREATER     12
#define ST_GREATER_EQ  13
#define ST_MINOR       14
#define ST_MINOR_EQ    15
#define ST_EQUAL       16
#define ST_EQUAL_EQ    17
#define ST_DIFF        18
#define ST_COMMENT     19
#define ST_PLUS        20
#define ST_MINUS       21
#define ST_MULT        22
#define ST_SEMICOLON   23
#define ST_COMMA       24
#define ST_OPAREN      25
#define ST_CPAREN      26
#define ST_OBRACK      27
#define ST_CBRACK      28
#define ST_OCURLY      29
#define ST_CCURLY      30
#define ST_SLASH       31

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
        else code = L_ID;
    }
    else if (estado == 11) {
        code = L_NUM;
    }
    else if (estado == 12) {
        code = L_GREATER;
    }
    else if (estado == 13) {
        code = L_GREATER_EQ;
    }
    else if (estado == 14) {
        code = L_MINOR;
    }
    else if (estado == 15) {
        code = L_MINOR_EQ;
    }
    else if (estado == 16) {
        code = L_EQUAL;
    }
    else if (estado == 17) {
        code = L_EQUAL_EQ;
    }
    else if (estado == 18) {
        code = L_DIFF;
    }
    else if (estado == 19) {
        code = L_COMMENT;
    }
    else if (estado == 20) {
        code = L_PLUS;
    }
    else if (estado == 21) {
        code = L_MINUS;
    }
    else if (estado == 22) {
        code = L_MULT;
    }
    else if (estado == 23) {
        code = L_SEMICOLON;
    }
    else if (estado == 24) {
        code = L_COMMA;
    }
    else if (estado == 25) {
        code = L_OPAREN;
    }
    else if (estado == 26) {
        code = L_CPAREN;
    }
    else if (estado == 27) {
        code = L_OBRACK;
    }
    else if (estado == 28) {
        code = L_CBRACK;
    }
    else if (estado == 29) {
        code = L_OCURLY;
    }
    else if (estado == 30) {
        code = L_CCURLY;
    }
    else if (estado == 31) {
        code = L_SLASH;
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
 *  Estados 10..31 são finais (ST_...), retornam diretamente o token
 * ------------------------------------------------------------------------- */

int T[32][20] = {
    /*                      Colunas:
       0   1      2        3        4        5         6         7         8         9
       L   D      +        -        *        ;         ,         (         )         [

      10  11    12       13       14       15        16        17        18        19
      ]   {      }        >        <        =         !         /         B         outro
    */

    /*------------------------------------------------------------------------------------*
     * Estado 0
     *------------------------------------------------------------------------------------*/
    {  1,  2,  ST_PLUS,  ST_MINUS, ST_MULT,  ST_SEMICOLON, ST_COMMA, ST_OPAREN, ST_CPAREN, ST_OBRACK,
       ST_CBRACK, ST_OCURLY, ST_CCURLY, 3,       4,          5,       6,        7,        0,       -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 1 (letra) => ainda formando um ID
     *------------------------------------------------------------------------------------*/
    {  1, -1,  ST_ID,    ST_ID,    ST_ID,    ST_ID,        ST_ID,    ST_ID,    ST_ID,     ST_ID,
       ST_ID,   ST_ID,   ST_ID,    ST_ID,    ST_ID,        ST_ID,    ST_ID,    ST_ID,     ST_ID,   -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 2 (dígito) => ainda formando um NUM
     *------------------------------------------------------------------------------------*/
    { -1,  2,  ST_NUM,   ST_NUM,   ST_NUM,   ST_NUM,       ST_NUM,   ST_NUM,   ST_NUM,    ST_NUM,
       ST_NUM,  ST_NUM,  ST_NUM,   ST_NUM,   ST_NUM,       ST_NUM,   ST_NUM,   ST_NUM,    ST_NUM,  -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 3 (>x)
     *------------------------------------------------------------------------------------*/
    { ST_GREATER, ST_GREATER, ST_GREATER,  ST_GREATER,  ST_GREATER, ST_GREATER, ST_GREATER, ST_GREATER, 
      ST_GREATER, ST_GREATER, ST_GREATER,  ST_GREATER,  ST_GREATER, ST_GREATER, ST_GREATER, ST_GREATER_EQ,
      ST_GREATER, ST_GREATER, ST_GREATER,  -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 4 (<x)
     *------------------------------------------------------------------------------------*/
    { ST_MINOR, ST_MINOR, ST_MINOR, ST_MINOR, ST_MINOR, ST_MINOR, ST_MINOR, ST_MINOR,
      ST_MINOR, ST_MINOR, ST_MINOR, ST_MINOR, ST_MINOR, ST_MINOR, ST_MINOR, ST_MINOR_EQ,
      ST_MINOR, ST_MINOR, ST_MINOR, -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 5 (=x)
     *------------------------------------------------------------------------------------*/
    { ST_EQUAL, ST_EQUAL, ST_EQUAL, ST_EQUAL, ST_EQUAL, ST_EQUAL, ST_EQUAL, ST_EQUAL,
      ST_EQUAL, ST_EQUAL, ST_EQUAL, ST_EQUAL, ST_EQUAL, ST_EQUAL, ST_EQUAL, ST_EQUAL_EQ,
      ST_EQUAL, ST_EQUAL, ST_EQUAL, -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 6 (!x)
     *------------------------------------------------------------------------------------*/
    { -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, ST_DIFF,
      -1, -1, -1, -1 },

    /*------------------------------------------------------------------------------------*
     * Estado 7 (/x)
     *------------------------------------------------------------------------------------*/
    { ST_SLASH, ST_SLASH, ST_SLASH, ST_SLASH, 8,        ST_SLASH, ST_SLASH, ST_SLASH,
      ST_SLASH, ST_SLASH, ST_SLASH, ST_SLASH, ST_SLASH, ST_SLASH, ST_SLASH, ST_SLASH,
      ST_SLASH, ST_SLASH, ST_SLASH, -1 },

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
      8, ST_COMMENT, 8, 8 },

    /*------------------------------------------------------------------------------------*
     * A partir daqui (10..31) são estados finais diretos (retornam token) => 0 em todas colunas
     *------------------------------------------------------------------------------------*/

    /* Estado 10: ST_ID */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 11: ST_NUM */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 12: ST_GREATER */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 13: ST_GREATER_EQ */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 14: ST_MINOR */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 15: ST_MINOR_EQ */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 16: ST_EQUAL */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 17: ST_EQUAL_EQ */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 18: ST_DIFF */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 19: ST_COMMENT */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 20: ST_PLUS */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 21: ST_MINUS */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 22: ST_MULT */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 23: ST_SEMICOLON */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 24: ST_COMMA */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 25: ST_OPAREN */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 26: ST_CPAREN */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 27: ST_OBRACK */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 28: ST_CBRACK */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 29: ST_OCURLY */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 30: ST_CCURLY */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    /* Estado 31: ST_SLASH */
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
