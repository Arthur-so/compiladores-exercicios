/* parser.y */
%{
#include <stdio.h>
#include <stdlib.h>
#include "lexeme.h"

int yylex();
void yyerror(const char *s);

/* O lexer será global, definido em main.c */
extern Lexer *lexer;
%}

/* Se quiser usar valores semânticos: */
%union {
  char *str;
  int ival;
}

/* Declare as tokens (Bison gera #define ID 258, etc.) */
%token <str> ID
%token <ival> NUM
%token IF ELSE INT RETURN VOID WHILE
%token PLUS MINUS MULT DIV
%token EQUAL EQUAL_EQUAL DIFFERENT
%token GREATER GREATER_EQUAL MINOR MINOR_EQUAL
%token SEMICOLON COMMA
%token OPEN_PARENTHESIS CLOSE_PARENTHESIS
%token OPEN_SQUARE_BRACKETS CLOSE_SQUARE_BRACKETS
%token OPEN_CURLY_BRACKETS CLOSE_CURLY_BRACKETS

%start programa

%%

programa:
    declaracao_lista
    ;

declaracao_lista:
    declaracao_lista declaracao
    | declaracao
    ;

declaracao:
    var_declaracao
    | fun_declaracao
    ;

var_declaracao:
    tipo_especificador ID SEMICOLON
    | tipo_especificador ID OPEN_SQUARE_BRACKETS NUM CLOSE_SQUARE_BRACKETS SEMICOLON
    ;

tipo_especificador:
    INT
    | VOID
    ;

fun_declaracao:
    tipo_especificador ID OPEN_PARENTHESIS params CLOSE_PARENTHESIS composto_decl
    ;

params:
    param_lista
    | VOID
    ;

param_lista:
    param_lista COMMA param
    | param
    ;

param:
    tipo_especificador ID
    | tipo_especificador ID OPEN_SQUARE_BRACKETS CLOSE_SQUARE_BRACKETS
    ;

composto_decl:
    OPEN_CURLY_BRACKETS local_declaracoes statement_lista CLOSE_CURLY_BRACKETS
    ;

local_declaracoes:
    local_declaracoes var_declaracao
    | /* vazio */
    ;

statement_lista:
    statement_lista statement
    | /* vazio */
    ;

statement:
    expressao_decl
    | composto_decl
    | selecao_decl
    | iteracao_decl
    | retorno_decl
    ;

expressao_decl:
    expressao SEMICOLON
    | SEMICOLON
    ;

selecao_decl:
    IF OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement
    | IF OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement ELSE statement
    ;

iteracao_decl:
    WHILE OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement
    ;

retorno_decl:
    RETURN SEMICOLON
    | RETURN expressao SEMICOLON
    ;

expressao:
    var EQUAL expressao
    | simples_expressao
    ;

var:
    ID
    | ID OPEN_SQUARE_BRACKETS expressao CLOSE_SQUARE_BRACKETS
    ;

simples_expressao:
    soma_expressao relacional soma_expressao
    | soma_expressao
    ;

relacional:
    MINOR_EQUAL
    | MINOR
    | GREATER
    | GREATER_EQUAL
    | EQUAL_EQUAL
    | DIFFERENT
    ;

soma_expressao:
    soma_expressao soma termo
    | termo
    ;

soma:
    PLUS
    | MINUS
    ;

termo:
    termo mult fator
    | fator
    ;

mult:
    MULT
    | DIV
    ;

fator:
    OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS
    | var
    | ativacao
    | NUM
    ;

ativacao:
    ID OPEN_PARENTHESIS args CLOSE_PARENTHESIS
    ;

args:
    arg_lista
    | /* vazio */
    ;

arg_lista:
    arg_lista COMMA expressao
    | expressao
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERRO SINTATICO: %s\n", s);
}
