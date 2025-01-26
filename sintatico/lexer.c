/* lexer.c */
#include "lexeme.h"

/* ------------------------------------------------------------------
   1) Defina códigos internos para os tokens finais do seu autômato.
      Não use "ID, NUM, IF, ELSE" etc. direto aqui, pois o Bison
      gera #define diferentes. Use, por exemplo, L_ID, L_NUM, etc.
   ------------------------------------------------------------------ */
#define L_KW_ELSE     100
#define L_KW_IF       101
#define L_KW_INT      102
#define L_KW_RETURN   103
#define L_KW_VOID     104
#define L_KW_WHILE    105

#define L_ID          110
#define L_NUM         111
#define L_GREATER     112
#define L_GREATER_EQ  113
#define L_MINOR       114
#define L_MINOR_EQ    115
#define L_EQUAL       116
#define L_EQUAL_EQ    117
#define L_DIFF        118
#define L_COMMENT     119
#define L_PLUS        120
#define L_MINUS       121
#define L_MULT        122
#define L_SEMICOLON   123
#define L_COMMA       124
#define L_OPAREN      125
#define L_CPAREN      126
#define L_OBRACK      127
#define L_CBRACK      128
#define L_OCURLY      129
#define L_CCURLY      130
#define L_SLASH       131

/* ------------------------------------------------------------------
   2) Implementação do Buffer
   ------------------------------------------------------------------ */
static void fill_buffer(FILE *fptr, Buffer *buffer) {
    memset(buffer->data, 0, buffer->size);
    if (fgets(buffer->data, buffer->size, fptr) != NULL) {
        buffer->next_char_id = 0;
    }
}

Buffer* allocate_buffer(int size) {
    Buffer* buffer = (Buffer*)malloc(sizeof(Buffer));
    if (!buffer) {
        fprintf(stderr, "Error allocating buffer struct.\n");
        exit(EXIT_FAILURE);
    }
    buffer->data = (char*)malloc(size);
    if (!buffer->data) {
        fprintf(stderr, "Error allocating buffer data.\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    memset(buffer->data, 0, size);
    buffer->next_char_id = 0;
    buffer->next_char_line = 1;
    buffer->size = size;
    buffer->new_line = 0;
    return buffer;
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

/* ------------------------------------------------------------------
   3) Inicialização do Lexer
   ------------------------------------------------------------------ */
Lexer* initialize_lexer(const char *filename, int buffer_size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return NULL;
    }

    Buffer *buffer = allocate_buffer(buffer_size);
    Token *token = (Token *)malloc(sizeof(Token));

    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    lexer->buffer = buffer;
    lexer->current_state = 0;
    lexer->current_lexeme[0] = '\0';
    lexer->file = file;
    lexer->token = token;
    lexer->token->error = 0;
    lexer->token->done = 0;

    return lexer;
}

void destroy_lexer(Lexer *lexer) {
    if (!lexer) return;
    deallocate_buffer(lexer->buffer);
    free(lexer->token);
    fclose(lexer->file);
    free(lexer);
}

/* ------------------------------------------------------------------
   4) Funções auxiliares de classificação
   ------------------------------------------------------------------ */
int get_tipo(char ch) {
    if (isalpha(ch)) {
        return 0; // Letra
    }
    else if (isdigit(ch)) {
        return 1; // Dígito
    }
    else {
        switch (ch) {
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
                    return 18; // Espaço ou EOF
                }
                else {
                    return 19; // Outro (não reconhecido)
                }
        }
    }
}

/*  classifica_lexema():
    Se for ID, verifica se é palavra-chave (if, else, etc.).
    Se for número, retorna L_NUM. Se for '+', retorna L_PLUS, etc.
    (Mas na verdade você já fez a distinção via autômato: 
     ID -> estado final 10, NUM-> estado final 11, etc.)
*/
int classifica_lexema(char* lexeme, int estado) {
    // Digamos que se estado == 10 => ID, se estado == 11 => NUM, etc.
    // Aí checamos manualmente se ID é "if", "else" etc.

    int token_code = estado; // de início, use o "estado" como token_code

    /* ID => checa keywords */
    if (estado == 10) {
        if (strcmp(lexeme, "else") == 0)       token_code = L_KW_ELSE;
        else if (strcmp(lexeme, "if") == 0)    token_code = L_KW_IF;
        else if (strcmp(lexeme, "int") == 0)   token_code = L_KW_INT;
        else if (strcmp(lexeme, "return") == 0)token_code = L_KW_RETURN;
        else if (strcmp(lexeme, "void") == 0)  token_code = L_KW_VOID;
        else if (strcmp(lexeme, "while") == 0) token_code = L_KW_WHILE;
        else                                   token_code = L_ID;
    }
    else if (estado == 11) {
        token_code = L_NUM;
    }
    else if (estado == 12) {
        token_code = L_GREATER;
    }
    else if (estado == 13) {
        token_code = L_GREATER_EQ;
    }
    else if (estado == 14) {
        token_code = L_MINOR;
    }
    else if (estado == 15) {
        token_code = L_MINOR_EQ;
    }
    else if (estado == 16) {
        token_code = L_EQUAL;
    }
    else if (estado == 17) {
        token_code = L_EQUAL_EQ;
    }
    else if (estado == 18) {
        token_code = L_DIFF;
    }
    else if (estado == 19) {
        token_code = L_COMMENT;
    }
    else if (estado == 20) {
        token_code = L_PLUS;
    }
    else if (estado == 21) {
        token_code = L_MINUS;
    }
    else if (estado == 22) {
        token_code = L_MULT;
    }
    else if (estado == 23) {
        token_code = L_SEMICOLON;
    }
    else if (estado == 24) {
        token_code = L_COMMA;
    }
    else if (estado == 25) {
        token_code = L_OPAREN;
    }
    else if (estado == 26) {
        token_code = L_CPAREN;
    }
    else if (estado == 27) {
        token_code = L_OBRACK;
    }
    else if (estado == 28) {
        token_code = L_CBRACK;
    }
    else if (estado == 29) {
        token_code = L_OCURLY;
    }
    else if (estado == 30) {
        token_code = L_CCURLY;
    }
    else if (estado == 31) {
        token_code = L_SLASH;
    }

    return token_code;
}

/* ------------------------------------------------------------------
   5) get_token(): roda o autômato e define token->token_code
   ------------------------------------------------------------------ */
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
        if (c == EOF) {
            lexer->token->done = 1;
            break;
        }
        /* Se estado == -1 => erro */
        if (lexer->current_state == -1) {
            aux_lexeme[char_lexeme_id++] = c;
            aux_lexeme[char_lexeme_id] = '\0';
            lexer->buffer->next_char_id++;

            lexer->token->error = 1;
            strncpy(lexer->token->lexeme, aux_lexeme, sizeof(lexer->token->lexeme));
            lexer->token->line = lexer->buffer->next_char_line;
            lexer->token->column = lexer->buffer->next_char_id;
            break;
        }
        /* Se estado atual é de aceitação => finaliza token */
        if (Aceita[lexer->current_state]) {
            aux_lexeme[char_lexeme_id] = '\0';

            lexer->token->token_code = classifica_lexema(aux_lexeme, lexer->current_state);
            lexer->token->line = lexer->buffer->next_char_line;
            lexer->token->column = lexer->buffer->next_char_id;
            strncpy(lexer->token->lexeme, aux_lexeme, sizeof(lexer->token->lexeme));
            
            lexer->current_state = 0;
            return lexer->token;
        }

        /* Lê próximo char */
        c = get_next_char(lexer->file, lexer->buffer);
        if (c == EOF) {
            lexer->token->done = 1;
            break;
        }

        char_code = get_tipo(c);
        novo_estado = T[lexer->current_state][char_code];
        avance = Avance[lexer->current_state][char_code];

        if (avance) {
            if (AdicionaAoToken[char_code] &&
                (AdicionaAoTokenEstado[novo_estado] || AdicionaAoTokenEstado[lexer->current_state]))
            {
                aux_lexeme[char_lexeme_id++] = c;
            }
            lexer->buffer->next_char_id++;
        }
        lexer->current_state = novo_estado;
    }

    /* Se chegou aqui e não aceitou nem EOF => erro */
    if (lexer->current_state != 0 && Aceita[lexer->current_state] == 0) {
        if (AdicionaAoTokenEstado[novo_estado] || AdicionaAoTokenEstado[lexer->current_state]) {
            aux_lexeme[char_lexeme_id++] = c;
            lexer->buffer->next_char_id++;
        }
        aux_lexeme[char_lexeme_id] = '\0';

        lexer->token->error = 1;
        strncpy(lexer->token->lexeme, aux_lexeme, sizeof(lexer->token->lexeme));
        lexer->token->line = lexer->buffer->next_char_line;
        lexer->token->column = lexer->buffer->next_char_id;
    }
    return lexer->token;
}

/* ------------------------------------------------------------------
   6) Tabelas do autômato
   ------------------------------------------------------------------ */

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

int T[32][20] = {
    /*  state 0, col 0..19 */
    {1,2,20,21,22,23,24,25,26,27,28,29,30,3,4,5,6,7,0,-1},
    {1,-1,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,-1},
    {-1,2,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,-1},
    {12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,13,12,12,12,-1},
    {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,15,14,14,14,-1},
    {16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,17,16,16,16,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,18,-1,-1,-1,-1},
    {31,31,31,31, 8,31,31,31,31,31,31,31,31,31,31,31,31,31,31,-1},
    { 8, 8, 8, 8, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,19, 8, 8},
    /* estado 10..31 => 0's */
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
    /* estados 10..31 => tudo 0 */
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
