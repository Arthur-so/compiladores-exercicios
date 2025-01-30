/* parser.y */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexeme.h"
#include "ast.h"

/* Precisa de yylex() e yyerror() */
int yylex();
void yyerror(const char *s);

/* Variáveis globais p/ armazenar o "último token lido" (p/ msg de erro) */
char g_last_lexeme[64];
int  g_last_line   = 1;
int  g_last_col    = 1;

/* Raiz da AST */
AST *g_root = NULL;
%}

/* AST como valor semântico das regras */
%union {
    AST *ast;   /* Para nós da AST */
    char *str;  /* Para lexemas de ID */
    int  ival;  /* Para valores de NUM */
}

/* Associa cada token ou não-terminal a um tipo do %union, se necessário */
%type <ast> programa declaracao_lista declaracao var_declaracao fun_declaracao tipo_especificador params param_lista param composto_decl local_declaracoes statement_lista statement expressao_decl selecao_decl iteracao_decl retorno_decl expressao var simples_expressao relacional soma_expressao soma termo mult fator ativacao args arg_lista

/* Tokens do Bison */
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

/* programa -> declaracao_lista */
programa:
    declaracao_lista
    {
        /* A raiz da AST é a lista de declarações */
        AST *node = newASTNodeLine("programa", g_last_line);
        addChild(node, $1);
        g_root = node;
    }
    ;

/* declaracao_lista -> declaracao_lista declaracao | declaracao */
declaracao_lista:
      declaracao_lista declaracao
      {
        AST *node = newASTNodeLine("declaracao_lista", g_last_line);
        addChild(node, $1);
        addChild(node, $2);
        $$ = node;
      }
    | declaracao
      {
        AST *node = newASTNodeLine("declaracao_lista", g_last_line);
        addChild(node, $1);
        $$ = node;
      }
    ;

/* declaração -> var_declaracao | fun_declaracao */
declaracao:
    var_declaracao
    {
      AST *node = newASTNodeLine("declaracao(var)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | fun_declaracao
    {
      AST *node = newASTNodeLine("declaracao(fun)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  ;

/* var_declaracao -> tipo_especificador ID ; 
                   | tipo_especificador ID [ NUM ] ; */
var_declaracao:
    tipo_especificador ID SEMICOLON
    {
      AST *node = newASTNodeLine("var_declaracao", g_last_line);
      addChild(node, $1);

      AST *idNode = newASTNodeLine($2, g_last_line);
      addChild(node, idNode);

      addChild(node, newASTNodeLine(";", g_last_line));
      free($2); /* libera a string do yylval.str */
      $$ = node;
    }
  | tipo_especificador ID OPEN_SQUARE_BRACKETS NUM CLOSE_SQUARE_BRACKETS SEMICOLON
    {
      AST *node = newASTNodeLine("var_declaracao_array", g_last_line);
      addChild(node, $1);

      AST *idNode = newASTNodeLine($2, g_last_line);
      addChild(node, idNode);

      addChild(node, newASTNodeLine("[", g_last_line));

      char buf[64];
      sprintf(buf, "NUM:%d", $4); 
      AST *numNode = newASTNodeLine(buf, g_last_line);
      addChild(node, numNode);

      addChild(node, newASTNodeLine("]", g_last_line));
      addChild(node, newASTNodeLine(";", g_last_line));

      free($2);
      $$ = node;
    }
    ;

/* tipo_especificador -> INT | VOID */
tipo_especificador:
    INT
    {
      AST *node = newASTNodeLine("INT", g_last_line);
      $$ = node;
    }
  | VOID
    {
      AST *node = newASTNodeLine("VOID", g_last_line);
      $$ = node;
    }
    ;

/* fun_declaracao -> tipo_especificador ID ( params ) composto_decl */
fun_declaracao:
    tipo_especificador ID OPEN_PARENTHESIS params CLOSE_PARENTHESIS composto_decl
    {
      AST *node = newASTNodeLine("fun_declaracao", g_last_line);
      addChild(node, $1);
      addChild(node, newASTNodeLine($2, g_last_line));
      addChild(node, newASTNodeLine("(", g_last_line));
      addChild(node, $4);
      addChild(node, newASTNodeLine(")", g_last_line));
      addChild(node, $6);
      free($2);
      $$ = node;
    }
    ;

/* params -> param_lista | VOID */
params:
    param_lista
    {
      $$ = $1;
    }
  | VOID
    {
      AST *node = newASTNodeLine("params_VOID", g_last_line);
      $$ = node;
    }
    ;

/* param_lista -> param_lista , param | param */
param_lista:
    param_lista COMMA param
    {
      AST *node = newASTNodeLine("param_lista", g_last_line);
      addChild(node, $1);
      addChild(node, newASTNodeLine(",", g_last_line));
      addChild(node, $3);
      $$ = node;
    }
  | param
    {
      AST *node = newASTNodeLine("param_lista", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
    ;

/* param -> tipo_especificador ID | tipo_especificador ID [] */
param:
    tipo_especificador ID
    {
      AST *node = newASTNodeLine("param", g_last_line);
      addChild(node, $1);
      addChild(node, newASTNodeLine($2, g_last_line));
      free($2);
      $$ = node;
    }
  | tipo_especificador ID OPEN_SQUARE_BRACKETS CLOSE_SQUARE_BRACKETS
    {
      AST *node = newASTNodeLine("param_array", g_last_line);
      addChild(node, $1);
      addChild(node, newASTNodeLine($2, g_last_line));
      addChild(node, newASTNodeLine("[", g_last_line));
      addChild(node, newASTNodeLine("]", g_last_line));
      free($2);
      $$ = node;
    }
    ;

/* composto_decl -> { local_declaracoes statement_lista } */
composto_decl:
    OPEN_CURLY_BRACKETS local_declaracoes statement_lista CLOSE_CURLY_BRACKETS
    {
      AST *node = newASTNodeLine("composto_decl", g_last_line);
      addChild(node, newASTNodeLine("{", g_last_line));
      addChild(node, $2);
      addChild(node, $3);
      addChild(node, newASTNodeLine("}", g_last_line));
      $$ = node;
    }
    ;

/* local_declaracoes -> local_declaracoes var_declaracao | vazio */
local_declaracoes:
    local_declaracoes var_declaracao
    {
      AST *node = newASTNodeLine("local_declaracoes", g_last_line);
      addChild(node, $1);
      addChild(node, $2);
      $$ = node;
    }
  | /* vazio */
    {
      AST *node = newASTNodeLine("local_declaracoes_vazio", g_last_line);
      $$ = node;
    }
    ;

/* statement_lista -> statement_lista statement | vazio */
statement_lista:
    statement_lista statement
    {
      AST *node = newASTNodeLine("statement_lista", g_last_line);
      addChild(node, $1);
      addChild(node, $2);
      $$ = node;
    }
  | /* vazio */
    {
      AST *node = newASTNodeLine("statement_lista_vazio", g_last_line);
      $$ = node;
    }
    ;

/* statement -> expressão_decl | composto_decl | selecao_decl | iteracao_decl | retorno_decl */
statement:
    expressao_decl
    {
      AST *node = newASTNodeLine("statement(expr)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | composto_decl
    {
      AST *node = newASTNodeLine("statement(composto)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | selecao_decl
    {
      AST *node = newASTNodeLine("statement(selecao)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | iteracao_decl
    {
      AST *node = newASTNodeLine("statement(iteracao)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | retorno_decl
    {
      AST *node = newASTNodeLine("statement(retorno)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
    ;

/* expressão_decl -> expressão ; | ; */
expressao_decl:
    expressao SEMICOLON
    {
      AST *node = newASTNodeLine("expressao_decl", g_last_line);
      addChild(node, $1);
      addChild(node, newASTNodeLine(";", g_last_line));
      $$ = node;
    }
  | SEMICOLON
    {
      AST *node = newASTNodeLine("expressao_decl_empty", g_last_line);
      addChild(node, newASTNodeLine(";", g_last_line));
      $$ = node;
    }
    ;

/* selecao_decl -> if ( expressao ) statement | if ( expressao ) statement else statement */
selecao_decl:
    IF OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement
    {
      AST *node = newASTNodeLine("selecao_if", g_last_line);
      addChild(node, newASTNodeLine("IF", g_last_line));
      addChild(node, newASTNodeLine("(", g_last_line));
      addChild(node, $3);
      addChild(node, newASTNodeLine(")", g_last_line));
      addChild(node, $5);
      $$ = node;
    }
  | IF OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement ELSE statement
    {
      AST *node = newASTNodeLine("selecao_if_else", g_last_line);
      addChild(node, newASTNodeLine("IF", g_last_line));
      addChild(node, newASTNodeLine("(", g_last_line));
      addChild(node, $3);
      addChild(node, newASTNodeLine(")", g_last_line));
      addChild(node, $5);
      addChild(node, newASTNodeLine("ELSE", g_last_line));
      addChild(node, $7);
      $$ = node;
    }
    ;

/* iteracao_decl -> while ( expressao ) statement */
iteracao_decl:
    WHILE OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement
    {
      AST *node = newASTNodeLine("iteracao_while", g_last_line);
      addChild(node, newASTNodeLine("WHILE", g_last_line));
      addChild(node, newASTNodeLine("(", g_last_line));
      addChild(node, $3);
      addChild(node, newASTNodeLine(")", g_last_line));
      addChild(node, $5);
      $$ = node;
    }
    ;

/* retorno_decl -> return ; | return expressao ; */
retorno_decl:
    RETURN SEMICOLON
    {
      AST *node = newASTNodeLine("retorno_decl", g_last_line);
      addChild(node, newASTNodeLine("RETURN", g_last_line));
      addChild(node, newASTNodeLine(";", g_last_line));
      $$ = node;
    }
  | RETURN expressao SEMICOLON
    {
      AST *node = newASTNodeLine("retorno_decl", g_last_line);
      addChild(node, newASTNodeLine("RETURN", g_last_line));
      addChild(node, $2);
      addChild(node, newASTNodeLine(";", g_last_line));
      $$ = node;
    }
    ;

/* expressão -> var = expressão | simples_expressao */
expressao:
    var EQUAL expressao
    {
      AST *node = newASTNodeLine("expressao_atribuicao", g_last_line);
      addChild(node, $1);
      addChild(node, newASTNodeLine("=", g_last_line));
      addChild(node, $3);
      $$ = node;
    }
  | simples_expressao
    {
      $$ = $1;
    }
    ;

/* var -> ID | ID [ expressao ] */
var:
    ID
    {
      AST *node = newASTNodeLine("var", g_last_line);
      addChild(node, newASTNodeLine($1, g_last_line));
      free($1);
      $$ = node;
    }
  | ID OPEN_SQUARE_BRACKETS expressao CLOSE_SQUARE_BRACKETS
    {
      AST *node = newASTNodeLine("var_array", g_last_line);
      addChild(node, newASTNodeLine($1, g_last_line));
      addChild(node, newASTNodeLine("[", g_last_line));
      addChild(node, $3);
      addChild(node, newASTNodeLine("]", g_last_line));
      free($1);
      $$ = node;
    }
    ;

/* simples_expressao -> soma_expressao relacional soma_expressao | soma_expressao */
simples_expressao:
    soma_expressao relacional soma_expressao
    {
      AST *node = newASTNodeLine("simples_expressao", g_last_line);
      addChild(node, $1);
      addChild(node, $2);
      addChild(node, $3);
      $$ = node;
    }
  | soma_expressao
    {
      $$ = $1;
    }
    ;

/* relacional -> <= | < | > | >= | == | != */
relacional:
    MINOR_EQUAL
    {
      $$ = newASTNodeLine("<=", g_last_line);
    }
  | MINOR
    {
      $$ = newASTNodeLine("<", g_last_line);
    }
  | GREATER
    {
      $$ = newASTNodeLine(">", g_last_line);
    }
  | GREATER_EQUAL
    {
      $$ = newASTNodeLine(">=", g_last_line);
    }
  | EQUAL_EQUAL
    {
      $$ = newASTNodeLine("==", g_last_line);
    }
  | DIFFERENT
    {
      $$ = newASTNodeLine("!=", g_last_line);
    }
    ;

/* soma_expressao -> soma_expressao soma termo | termo */
soma_expressao:
    soma_expressao soma termo
    {
      AST *node = newASTNodeLine("soma_expressao", g_last_line);
      addChild(node, $1);
      addChild(node, $2);
      addChild(node, $3);
      $$ = node;
    }
  | termo
    {
      $$ = $1;
    }
    ;

/* soma -> + | - */
soma:
    PLUS
    {
      $$ = newASTNodeLine("+", g_last_line);
    }
  | MINUS
    {
      $$ = newASTNodeLine("-", g_last_line);
    }
    ;

/* termo -> termo mult fator | fator */
termo:
    termo mult fator
    {
      AST *node = newASTNodeLine("termo", g_last_line);
      addChild(node, $1);
      addChild(node, $2);
      addChild(node, $3);
      $$ = node;
    }
  | fator
    {
      $$ = $1;
    }
    ;

/* mult -> * | / */
mult:
    MULT
    {
      $$ = newASTNodeLine("*", g_last_line);
    }
  | DIV
    {
      $$ = newASTNodeLine("/", g_last_line);
    }
    ;

/* fator -> ( expressao ) | var | ativacao | NUM */
fator:
    OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS
    {
      AST *node = newASTNodeLine("fator(paren)", g_last_line);
      addChild(node, newASTNodeLine("(", g_last_line));
      addChild(node, $2);
      addChild(node, newASTNodeLine(")", g_last_line));
      $$ = node;
    }
  | var
    {
      AST *node = newASTNodeLine("fator(var)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | ativacao
    {
      AST *node = newASTNodeLine("fator(ativ)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | NUM
    {
      char buf[64];
      sprintf(buf, "NUM:%d", $1);
      AST *node = newASTNodeLine("fator(NUM)", g_last_line);
      addChild(node, newASTNodeLine(buf, g_last_line));
      $$ = node;
    }
    ;

/* ativacao -> ID ( args ) */
ativacao:
    ID OPEN_PARENTHESIS args CLOSE_PARENTHESIS
    {
      AST *node = newASTNodeLine("ativacao", g_last_line);
      addChild(node, newASTNodeLine($1, g_last_line));
      addChild(node, newASTNodeLine("(", g_last_line));
      addChild(node, $3);
      addChild(node, newASTNodeLine(")", g_last_line));
      free($1);
      $$ = node;
    }
    ;

/* args -> arg_lista | vazio */
args:
    arg_lista
    {
      $$ = $1;
    }
  |
    {
      AST *node = newASTNodeLine("args_vazio", g_last_line);
      $$ = node;
    }
    ;

/* arg_lista -> arg_lista , expressao | expressao */
arg_lista:
    arg_lista COMMA expressao
    {
      AST *node = newASTNodeLine("arg_lista", g_last_line);
      addChild(node, $1);
      addChild(node, newASTNodeLine(",", g_last_line));
      addChild(node, $3);
      $$ = node;
    }
  | expressao
    {
      AST *node = newASTNodeLine("arg_lista", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
    ;

%%

/* Erro sintático */
void yyerror(const char *s) {
    fprintf(stderr, 
      "ERRO SINTATICO: \"%s\" INVALIDO [linha: %d], COLUNA %d\n",
      g_last_lexeme, g_last_line, g_last_col
    );
}
