/* lexer_adapter.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Inclua o "ast.h" ANTES do "parser.tab.h" */
#include "ast.h"
#include "parser.tab.h"
#include "lexeme.h"


/* Precisamos acessar g_last_* do parser.y */
extern char g_last_lexeme[64];
extern int  g_last_line;
extern int  g_last_col;

/* O lexer global vem de main.c */
extern Lexer *lexer;

/* Precisamos do yylval se quisermos setar valores semânticos */
extern YYSTYPE yylval;

/* Definições internas (os mesmos #define do lexer.c) */
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

    /* Fim de arquivo => return 0 p/ Bison encerrar */
    if (t->done) {
        return 0;
    }

    /* Se erro léxico */
    if (t->error) {
        fprintf(stderr, 
            "ERRO LEXICO: \"%s\" INVALIDO [linha: %d], COLUNA %d\n",
             t->lexeme, t->line, t->column);
        return 0;
    }

    /* Salva o último token lido (p/ erro sintático) */
    strncpy(g_last_lexeme, t->lexeme, 63);
    g_last_line = t->line;
    g_last_col  = t->column;

    /* Se for comentário, ignorar e ler próximo */
    if (t->token_code == L_COMMENT) {
        return yylex();
    }

    switch (t->token_code) {
        case L_KW_ELSE:   return ELSE;
        case L_KW_IF:     return IF;
        case L_KW_INT:    return INT;
        case L_KW_RETURN: return RETURN;
        case L_KW_VOID:   return VOID;
        case L_KW_WHILE:  return WHILE;

        case L_ID:
            /* yylval.str => uma cópia do lexeme */
            yylval.str = strdup(t->lexeme);
            return ID;

        case L_NUM:
            /* yylval.ival => valor do número */
            yylval.ival = atoi(t->lexeme);
            return NUM;

        case L_PLUS:       return PLUS;
        case L_MINUS:      return MINUS;
        case L_MULT:       return MULT;
        case L_SLASH:      return DIV;
        case L_EQUAL:      return EQUAL;
        case L_EQUAL_EQ:   return EQUAL_EQUAL;
        case L_DIFF:       return DIFFERENT;
        case L_GREATER:    return GREATER;
        case L_GREATER_EQ: return GREATER_EQUAL;
        case L_MINOR:      return MINOR;
        case L_MINOR_EQ:   return MINOR_EQUAL;
        case L_SEMICOLON:  return SEMICOLON;
        case L_COMMA:      return COMMA;
        case L_OPAREN:     return OPEN_PARENTHESIS;
        case L_CPAREN:     return CLOSE_PARENTHESIS;
        case L_OBRACK:     return OPEN_SQUARE_BRACKETS;
        case L_CBRACK:     return CLOSE_SQUARE_BRACKETS;
        case L_OCURLY:     return OPEN_CURLY_BRACKETS;
        case L_CCURLY:     return CLOSE_CURLY_BRACKETS;

        default:
            fprintf(stderr, "TOKEN DESCONHECIDO: code=%d lex=\"%s\"\n",
                    t->token_code, t->lexeme);
            return 0;
    }
}
