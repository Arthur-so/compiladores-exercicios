%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexeme.h"
#include "ast.h"

/* Precisamos de yylex() e yyerror() */
int yylex();
void yyerror(const char *s);

/* Variáveis globais p/ armazenar o "último token lido" (para msg de erro) */
char g_last_lexeme[64];
int  g_last_line   = 1;
int  g_last_col    = 1;

/* Raiz da AST final */
AST *g_root = NULL;

/* Se você estiver usando 'extern Lexer *lexer;' 
   pode declará-lo aqui ou em outro lugar, por exemplo:
   extern Lexer *lexer;
*/
%}

/* Se quisermos AST como valor semântico das regras... */
%union {
    AST *ast;   /* Para nós da AST */
    char *str;  /* Para lexemas de ID */
    int  ival;  /* Para valores de NUM */
}

/* Associa cada token ou não-terminal a um tipo do %union, se necessário */
%type <ast> programa declaracao_lista declaracao var_declaracao fun_declaracao tipo_especificador params param_lista param composto_decl local_declaracoes statement_lista statement expressao_decl selecao_decl iteracao_decl retorno_decl expressao var simples_expressao relacional soma_expressao soma termo mult fator ativacao args arg_lista

/* Tokens do Bison (Bison vai gerar #define ID 258, NUM 259, etc.) */
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

/* ------------------------------------------------------------------
   GRAMÁTICA DO C– (com construção de AST)
   ------------------------------------------------------------------ */

/* programa -> declaracao_lista */
programa:
    declaracao_lista
    {
        /* A raiz da AST é a lista de declarações */
        g_root = $1;
    }
    ;

/* declaracao_lista -> declaracao_lista declaracao | declaracao */
declaracao_lista:
      declaracao_lista declaracao
      {
        AST *node = newASTNode("declaracao_lista");
        addChild(node, $1);
        addChild(node, $2);
        $$ = node;
      }
    | declaracao
      {
        AST *node = newASTNode("declaracao_lista");
        addChild(node, $1);
        $$ = node;
      }
    ;

/* declaração -> var_declaracao | fun_declaracao */
declaracao:
    var_declaracao
    {
      AST *node = newASTNode("declaracao(var)");
      addChild(node, $1);
      $$ = node;
    }
  | fun_declaracao
    {
      AST *node = newASTNode("declaracao(fun)");
      addChild(node, $1);
      $$ = node;
    }
  ;

/* var_declaracao -> tipo_especificador ID ; | tipo_especificador ID [ NUM ] ; */
var_declaracao:
    tipo_especificador ID SEMICOLON
    {
      AST *node = newASTNode("var_declaracao");
      addChild(node, $1);
      AST *idNode = newASTNode($2);  /* $2 = string do ID */
      addChild(node, idNode);
      addChild(node, newASTNode(";"));
      free($2); /* libera a string do yylval.str */
      $$ = node;
    }
  | tipo_especificador ID OPEN_SQUARE_BRACKETS NUM CLOSE_SQUARE_BRACKETS SEMICOLON
    {
      AST *node = newASTNode("var_declaracao_array");
      addChild(node, $1);

      AST *idNode = newASTNode($2);
      addChild(node, idNode);
      addChild(node, newASTNode("["));

      char buf[64];
      sprintf(buf, "NUM:%d", $4); 
      AST *numNode = newASTNode(buf);
      addChild(node, numNode);

      addChild(node, newASTNode("]"));
      addChild(node, newASTNode(";"));

      free($2);
      $$ = node;
    }
    ;

/* tipo_especificador -> INT | VOID */
tipo_especificador:
    INT
    {
      AST *node = newASTNode("INT");
      $$ = node;
    }
  | VOID
    {
      AST *node = newASTNode("VOID");
      $$ = node;
    }
    ;

/* fun_declaracao -> tipo_especificador ID ( params ) composto_decl */
fun_declaracao:
    tipo_especificador ID OPEN_PARENTHESIS params CLOSE_PARENTHESIS composto_decl
    {
      AST *node = newASTNode("fun_declaracao");
      addChild(node, $1);
      addChild(node, newASTNode($2));
      addChild(node, newASTNode("("));
      addChild(node, $4);
      addChild(node, newASTNode(")"));
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
      AST *node = newASTNode("params_VOID");
      $$ = node;
    }
    ;

/* param_lista -> param_lista , param | param */
param_lista:
    param_lista COMMA param
    {
      AST *node = newASTNode("param_lista");
      addChild(node, $1);
      addChild(node, newASTNode(","));
      addChild(node, $3);
      $$ = node;
    }
  | param
    {
      AST *node = newASTNode("param_lista");
      addChild(node, $1);
      $$ = node;
    }
    ;

/* param -> tipo_especificador ID | tipo_especificador ID [] */
param:
    tipo_especificador ID
    {
      AST *node = newASTNode("param");
      addChild(node, $1);
      addChild(node, newASTNode($2));
      free($2);
      $$ = node;
    }
  | tipo_especificador ID OPEN_SQUARE_BRACKETS CLOSE_SQUARE_BRACKETS
    {
      AST *node = newASTNode("param_array");
      addChild(node, $1);
      addChild(node, newASTNode($2));
      addChild(node, newASTNode("["));
      addChild(node, newASTNode("]"));
      free($2);
      $$ = node;
    }
    ;

/* composto_decl -> { local_declaracoes statement_lista } */
composto_decl:
    OPEN_CURLY_BRACKETS local_declaracoes statement_lista CLOSE_CURLY_BRACKETS
    {
      AST *node = newASTNode("composto_decl");
      addChild(node, newASTNode("{"));
      addChild(node, $2);
      addChild(node, $3);
      addChild(node, newASTNode("}"));
      $$ = node;
    }
    ;

/* local_declaracoes -> local_declaracoes var_declaracao | vazio */
local_declaracoes:
    local_declaracoes var_declaracao
    {
      AST *node = newASTNode("local_declaracoes");
      addChild(node, $1);
      addChild(node, $2);
      $$ = node;
    }
  | /* vazio */
    {
      AST *node = newASTNode("local_declaracoes_vazio");
      $$ = node;
    }
    ;

/* statement_lista -> statement_lista statement | vazio */
statement_lista:
    statement_lista statement
    {
      AST *node = newASTNode("statement_lista");
      addChild(node, $1);
      addChild(node, $2);
      $$ = node;
    }
  | /* vazio */
    {
      AST *node = newASTNode("statement_lista_vazio");
      $$ = node;
    }
    ;

/* statement -> expressão_decl | composto_decl | selecao_decl | iteracao_decl | retorno_decl */
statement:
    expressao_decl
    {
      AST *node = newASTNode("statement(expr)");
      addChild(node, $1);
      $$ = node;
    }
  | composto_decl
    {
      AST *node = newASTNode("statement(composto)");
      addChild(node, $1);
      $$ = node;
    }
  | selecao_decl
    {
      AST *node = newASTNode("statement(selecao)");
      addChild(node, $1);
      $$ = node;
    }
  | iteracao_decl
    {
      AST *node = newASTNode("statement(iteracao)");
      addChild(node, $1);
      $$ = node;
    }
  | retorno_decl
    {
      AST *node = newASTNode("statement(retorno)");
      addChild(node, $1);
      $$ = node;
    }
    ;

/* expressão_decl -> expressão ; | ; */
expressao_decl:
    expressao SEMICOLON
    {
      AST *node = newASTNode("expressao_decl");
      addChild(node, $1);
      addChild(node, newASTNode(";"));
      $$ = node;
    }
  | SEMICOLON
    {
      AST *node = newASTNode("expressao_decl_empty");
      addChild(node, newASTNode(";"));
      $$ = node;
    }
    ;

/* selecao_decl -> if ( expressao ) statement | if ( expressao ) statement else statement */
selecao_decl:
    IF OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement
    {
      AST *node = newASTNode("selecao_if");
      addChild(node, newASTNode("IF"));
      addChild(node, newASTNode("("));
      addChild(node, $3);
      addChild(node, newASTNode(")"));
      addChild(node, $5);
      $$ = node;
    }
  | IF OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement ELSE statement
    {
      AST *node = newASTNode("selecao_if_else");
      addChild(node, newASTNode("IF"));
      addChild(node, newASTNode("("));
      addChild(node, $3);
      addChild(node, newASTNode(")"));
      addChild(node, $5);
      addChild(node, newASTNode("ELSE"));
      addChild(node, $7);
      $$ = node;
    }
    ;

/* iteracao_decl -> while ( expressao ) statement */
iteracao_decl:
    WHILE OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement
    {
      AST *node = newASTNode("iteracao_while");
      addChild(node, newASTNode("WHILE"));
      addChild(node, newASTNode("("));
      addChild(node, $3);
      addChild(node, newASTNode(")"));
      addChild(node, $5);
      $$ = node;
    }
    ;

/* retorno_decl -> return ; | return expressao ; */
retorno_decl:
    RETURN SEMICOLON
    {
      AST *node = newASTNode("retorno_decl");
      addChild(node, newASTNode("RETURN"));
      addChild(node, newASTNode(";"));
      $$ = node;
    }
  | RETURN expressao SEMICOLON
    {
      AST *node = newASTNode("retorno_decl");
      addChild(node, newASTNode("RETURN"));
      addChild(node, $2);
      addChild(node, newASTNode(";"));
      $$ = node;
    }
    ;

/* expressão -> var = expressão | simples_expressao */
expressao:
    var EQUAL expressao
    {
      AST *node = newASTNode("expressao_atribuicao");
      addChild(node, $1);
      addChild(node, newASTNode("="));
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
      AST *node = newASTNode("var");
      addChild(node, newASTNode($1));
      free($1);
      $$ = node;
    }
  | ID OPEN_SQUARE_BRACKETS expressao CLOSE_SQUARE_BRACKETS
    {
      AST *node = newASTNode("var_array");
      addChild(node, newASTNode($1));
      addChild(node, newASTNode("["));
      addChild(node, $3);
      addChild(node, newASTNode("]"));
      free($1);
      $$ = node;
    }
    ;

/* simples_expressao -> soma_expressao relacional soma_expressao | soma_expressao */
simples_expressao:
    soma_expressao relacional soma_expressao
    {
      AST *node = newASTNode("simples_expressao");
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
      $$ = newASTNode("<=");
    }
  | MINOR
    {
      $$ = newASTNode("<");
    }
  | GREATER
    {
      $$ = newASTNode(">");
    }
  | GREATER_EQUAL
    {
      $$ = newASTNode(">=");
    }
  | EQUAL_EQUAL
    {
      $$ = newASTNode("==");
    }
  | DIFFERENT
    {
      $$ = newASTNode("!=");
    }
    ;

/* soma_expressao -> soma_expressao soma termo | termo */
soma_expressao:
    soma_expressao soma termo
    {
      AST *node = newASTNode("soma_expressao");
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
      $$ = newASTNode("+");
    }
  | MINUS
    {
      $$ = newASTNode("-");
    }
    ;

/* termo -> termo mult fator | fator */
termo:
    termo mult fator
    {
      AST *node = newASTNode("termo");
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
      $$ = newASTNode("*");
    }
  | DIV
    {
      $$ = newASTNode("/");
    }
    ;

/* fator -> ( expressao ) | var | ativacao | NUM */
fator:
    OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS
    {
      AST *node = newASTNode("fator(paren)");
      addChild(node, newASTNode("("));
      addChild(node, $2);
      addChild(node, newASTNode(")"));
      $$ = node;
    }
  | var
    {
      AST *node = newASTNode("fator(var)");
      addChild(node, $1);
      $$ = node;
    }
  | ativacao
    {
      AST *node = newASTNode("fator(ativ)");
      addChild(node, $1);
      $$ = node;
    }
  | NUM
    {
      /* Ex: "NUM:123" */
      char buf[64];
      sprintf(buf, "NUM:%d", $1);
      AST *node = newASTNode("fator(NUM)");
      addChild(node, newASTNode(buf));
      $$ = node;
    }
    ;

/* ativacao -> ID ( args ) */
ativacao:
    ID OPEN_PARENTHESIS args CLOSE_PARENTHESIS
    {
      AST *node = newASTNode("ativacao");
      addChild(node, newASTNode($1));
      addChild(node, newASTNode("("));
      addChild(node, $3);
      addChild(node, newASTNode(")"));
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
  | /* vazio */
    {
      AST *node = newASTNode("args_vazio");
      $$ = node;
    }
    ;

/* arg_lista -> arg_lista , expressao | expressao */
arg_lista:
    arg_lista COMMA expressao
    {
      AST *node = newASTNode("arg_lista");
      addChild(node, $1);
      addChild(node, newASTNode(","));
      addChild(node, $3);
      $$ = node;
    }
  | expressao
    {
      AST *node = newASTNode("arg_lista");
      addChild(node, $1);
      $$ = node;
    }
    ;

%%

/* Rotina de erro sintático */
void yyerror(const char *s) {
    /* Você pode imprimir algo como: 
       ERRO SINTATICO: "<token>" INVALIDO [linha X], COLUNA Y */
    fprintf(stderr, 
      "ERRO SINTATICO: \"%s\" INVALIDO [linha: %d], COLUNA %d\n",
      g_last_lexeme, g_last_line, g_last_col
    );
}
