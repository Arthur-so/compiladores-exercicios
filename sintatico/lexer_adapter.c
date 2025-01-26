/* lexer_adapter.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"   /* gerado por bison -d parser.y */
#include "lexeme.h"

/* Precisamos do yylval se quisermos atribuir valores semânticos */
extern YYSTYPE yylval;

/* O lexer global vem de main.c */
extern Lexer *lexer;

/* Definições dos códigos internos que o lexer atribui (mesmos #define de lexer.c) */
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

int yylex(void) {
    Token *t = get_token(lexer);

    if (t->done) {
        /* Fim de arquivo => retorne 0 para o Bison */
        return 0;
    }

    if (t->error) {
        fprintf(stderr, "ERRO LÉXICO: \"%s\" inválido [linha %d, col %d]\n",
                t->lexeme, t->line, t->column);
        return 0; // ou retornar um token de erro
    }

    /* Se for comentário, ignore e chame novamente */
    if (t->token_code == L_COMMENT) {
        return yylex();
    }

    /* Ajustar valor semântico (ex: se ID => yylval.str = strdup(...) ) */
    switch (t->token_code) {
        case L_KW_ELSE:       return ELSE;
        case L_KW_IF:         return IF;
        case L_KW_INT:        return INT;
        case L_KW_RETURN:     return RETURN;
        case L_KW_VOID:       return VOID;
        case L_KW_WHILE:      return WHILE;

        case L_ID:
            yylval.str = strdup(t->lexeme);
            return ID;

        case L_NUM:
            yylval.ival = atoi(t->lexeme);
            return NUM;

        case L_GREATER:       return GREATER;
        case L_GREATER_EQ:    return GREATER_EQUAL;
        case L_MINOR:         return MINOR;
        case L_MINOR_EQ:      return MINOR_EQUAL;
        case L_EQUAL:         return EQUAL;
        case L_EQUAL_EQ:      return EQUAL_EQUAL;
        case L_DIFF:          return DIFFERENT;
        case L_PLUS:          return PLUS;
        case L_MINUS:         return MINUS;
        case L_MULT:          return MULT;
        case L_SLASH:         return DIV;
        case L_SEMICOLON:     return SEMICOLON;
        case L_COMMA:         return COMMA;
        case L_OPAREN:        return OPEN_PARENTHESIS;
        case L_CPAREN:        return CLOSE_PARENTHESIS;
        case L_OBRACK:        return OPEN_SQUARE_BRACKETS;
        case L_CBRACK:        return CLOSE_SQUARE_BRACKETS;
        case L_OCURLY:        return OPEN_CURLY_BRACKETS;
        case L_CCURLY:        return CLOSE_CURLY_BRACKETS;
        
        default:
            fprintf(stderr, "TOKEN DESCONHECIDO: code=%d lexeme='%s'\n",
                    t->token_code, t->lexeme);
            return 0;
    }
}
