%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexeme.h"
#include "ast.h"

/* Declarações necessárias para o Bison */
int yylex();
void yyerror(const char *s);

/* Variáveis globais para armazenar o "último token lido" (para mensagens de erro) */
char g_last_lexeme[64];
int  g_last_line   = 1;
int  g_last_col    = 1;

/* Raiz da AST */
AST *g_root = NULL;
%}

/* Definição do tipo de valor semântico */
%union {
    AST *ast;   /* Para nós da AST */
    char *str;  /* Para lexemas de ID */
    int  ival;  /* Para valores de NUM */
}

/* Associando tokens e não-terminais com tipos do %union */
%type <ast> programa declaracao_lista declaracao var_declaracao fun_declaracao tipo_especificador params param_lista param composto_decl local_declaracoes statement_lista statement expressao_decl selecao_decl iteracao_decl retorno_decl expressao var simples_expressao relacional soma_expressao soma termo mult fator ativacao args arg_lista

/* Definição dos tokens */
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

/* Início da gramática */
%start programa

%%

/* Regra principal: programa -> declaracao_lista */
programa:
    declaracao_lista
    {
        /* Cria o nó raiz da AST */
        AST *node = newASTNodeLine("programa", g_last_line);
        addChild(node, $1);
        g_root = node;
    }
    ;

/* Regra para a lista de declarações: declaracao_lista -> declaracao_lista declaracao | declaracao */
declaracao_lista:
      declaracao_lista declaracao
      {
        /* Cria nó para lista de declarações e adiciona filhos */
        AST *node = newASTNodeLine("declaracao_lista", g_last_line);
        addChild(node, $1);
        addChild(node, $2);
        $$ = node;
      }
    | declaracao
      {
        /* Cria nó para lista de declarações com uma única declaração */
        AST *node = newASTNodeLine("declaracao_lista", g_last_line);
        addChild(node, $1);
        $$ = node;
      }
    ;

/* Regra para uma declaração: declaracao -> var_declaracao | fun_declaracao */
declaracao:
    var_declaracao
    {
      /* Cria nó para declaração de variável */
      AST *node = newASTNodeLine("declaracao(var)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | fun_declaracao
    {
      /* Cria nó para declaração de função */
      AST *node = newASTNodeLine("declaracao(fun)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  ;

/* Regra para declaração de variável: var_declaracao -> tipo_especificador ID ; | tipo_especificador ID [ NUM ] ; */
var_declaracao:
    tipo_especificador ID SEMICOLON
    {
      /* Cria nó para declaração de variável simples */
      AST *node = newASTNodeLine("var_declaracao", g_last_line);
      addChild(node, $1);

      AST *idNode = newASTNodeLine($2, g_last_line);
      addChild(node, idNode);

      /* Removido: addChild(node, newASTNodeLine(";", g_last_line)); */
      free($2); /* Libera a string do ID */
      $$ = node;
    }
  | tipo_especificador ID OPEN_SQUARE_BRACKETS NUM CLOSE_SQUARE_BRACKETS SEMICOLON
    {
      /* Cria nó para declaração de vetor */
      AST *node = newASTNodeLine("var_declaracao_array", g_last_line);
      addChild(node, $1);

      AST *idNode = newASTNodeLine($2, g_last_line);
      addChild(node, idNode);

      /* Removidos: addChild(node, newASTNodeLine("[", g_last_line));
         addChild(node, newASTNodeLine("]", g_last_line));
         addChild(node, newASTNodeLine(";", g_last_line)); */

      /* Adiciona o número do tamanho do vetor */
      char buf[64];
      sprintf(buf, "NUM:%d", $4); 
      AST *numNode = newASTNodeLine(buf, g_last_line);
      addChild(node, numNode);

      free($2); /* Libera a string do ID */
      $$ = node;
    }
    ;

/* Regra para especificador de tipo: tipo_especificador -> INT | VOID */
tipo_especificador:
    INT
    {
      /* Cria nó para tipo INT */
      AST *node = newASTNodeLine("INT", g_last_line);
      $$ = node;
    }
  | VOID
    {
      /* Cria nó para tipo VOID */
      AST *node = newASTNodeLine("VOID", g_last_line);
      $$ = node;
    }
    ;

/* Regra para declaração de função: fun_declaracao -> tipo_especificador ID ( params ) composto_decl */
fun_declaracao:
    tipo_especificador ID OPEN_PARENTHESIS params CLOSE_PARENTHESIS composto_decl
    {
      /* Cria nó para declaração de função */
      AST *node = newASTNodeLine("fun_declaracao", g_last_line);
      addChild(node, $1);
      addChild(node, newASTNodeLine($2, g_last_line));

      /* Removidos: addChild(node, newASTNodeLine("(", g_last_line));
         addChild(node, newASTNodeLine(")", g_last_line)); */

      addChild(node, $4); /* Adiciona parâmetros */
      addChild(node, $6); /* Adiciona corpo da função */
      free($2); /* Libera a string do ID */
      $$ = node;
    }
    ;

/* Regra para parâmetros: params -> param_lista | VOID */
params:
    param_lista
    {
      /* Passa a lista de parâmetros */
      $$ = $1;
    }
  | VOID
    {
      /* Cria nó indicando que não há parâmetros */
      AST *node = newASTNodeLine("params_VOID", g_last_line);
      $$ = node;
    }
    ;

/* Regra para lista de parâmetros: param_lista -> param_lista , param | param */
param_lista:
    param_lista COMMA param
    {
      /* Cria nó para lista de parâmetros e adiciona o novo parâmetro */
      AST *node = newASTNodeLine("param_lista", g_last_line);
      addChild(node, $1);
      /* Removido: addChild(node, newASTNodeLine(",", g_last_line)); */
      addChild(node, $3);
      $$ = node;
    }    
  | param
    {
      /* Cria nó para lista de parâmetros com um único parâmetro */
      AST *node = newASTNodeLine("param_lista", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
    ;

/* Regra para um parâmetro: param -> tipo_especificador ID | tipo_especificador ID [] */
param:
    tipo_especificador ID
    {
      /* Cria nó para parâmetro simples */
      AST *node = newASTNodeLine("param", g_last_line);
      addChild(node, $1);
      AST *idNode = newASTNodeLine($2, g_last_line);
      addChild(node, idNode);
      free($2); /* Libera a string do ID */
      $$ = node;
    }
  | tipo_especificador ID OPEN_SQUARE_BRACKETS CLOSE_SQUARE_BRACKETS
    {
      /* Cria nó para parâmetro que é um vetor */
      AST *node = newASTNodeLine("param_array", g_last_line);
      addChild(node, $1);
      AST *idNode = newASTNodeLine($2, g_last_line);
      addChild(node, idNode);
      /* Removidos: addChild(node, newASTNodeLine("[", g_last_line));
         addChild(node, newASTNodeLine("]", g_last_line)); */
      free($2); /* Libera a string do ID */
      $$ = node;
    }
    ;

/* Regra para corpo composto: composto_decl -> { local_declaracoes statement_lista } */
composto_decl:
    OPEN_CURLY_BRACKETS local_declaracoes statement_lista CLOSE_CURLY_BRACKETS
    {
      /* Cria nó para corpo composto da função */
      AST *node = newASTNodeLine("composto_decl", g_last_line);
      /* Removidos: addChild(node, newASTNodeLine("{", g_last_line));
         addChild(node, newASTNodeLine("}", g_last_line)); */
      addChild(node, $2); /* Adiciona declarações locais */
      addChild(node, $3); /* Adiciona lista de statements */
      $$ = node;
    }
    ;

/* Regra para declarações locais: local_declaracoes -> local_declaracoes var_declaracao | vazio */
local_declaracoes:
    local_declaracoes var_declaracao
    {
      /* Cria nó para declarações locais e adiciona a nova declaração */
      AST *node = newASTNodeLine("local_declaracoes", g_last_line);
      addChild(node, $1);
      addChild(node, $2);
      $$ = node;
    }
  | /* vazio */
    {
      /* Cria nó indicando que não há declarações locais */
      AST *node = newASTNodeLine("local_declaracoes_vazio", g_last_line);
      $$ = node;
    }
    ;

/* Regra para lista de statements: statement_lista -> statement_lista statement | vazio */
statement_lista:
    statement_lista statement
    {
      /* Cria nó para lista de statements e adiciona o novo statement */
      AST *node = newASTNodeLine("statement_lista", g_last_line);
      addChild(node, $1);
      addChild(node, $2);
      $$ = node;
    }
  | /* vazio */
    {
      /* Cria nó indicando que não há statements */
      AST *node = newASTNodeLine("statement_lista_vazio", g_last_line);
      $$ = node;
    }
    ;

/* Regra para um statement: statement -> expressão_decl | composto_decl | selecao_decl | iteracao_decl | retorno_decl */
statement:
    expressao_decl
    {
      /* Cria nó para statement de expressão */
      AST *node = newASTNodeLine("statement(expr)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | composto_decl
    {
      /* Cria nó para statement composto */
      AST *node = newASTNodeLine("statement(composto)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | selecao_decl
    {
      /* Cria nó para statement de seleção */
      AST *node = newASTNodeLine("statement(selecao)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | iteracao_decl
    {
      /* Cria nó para statement de iteração */
      AST *node = newASTNodeLine("statement(iteracao)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | retorno_decl
    {
      /* Cria nó para statement de retorno */
      AST *node = newASTNodeLine("statement(retorno)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
    ;

/* Regra para declaração de expressão: expressão_decl -> expressão ; | ; */
expressao_decl:
    expressao SEMICOLON
    {
      /* Cria nó para declaração de expressão */
      AST *node = newASTNodeLine("expressao_decl", g_last_line);
      addChild(node, $1);
      /* Removido: addChild(node, newASTNodeLine(";", g_last_line)); */
      $$ = node;
    }
  | SEMICOLON
    {
      /* Representa uma declaração de expressão vazia; retorna NULL para evitar nós desnecessários */
      $$ = NULL;
    }
    ;

/* Regra para seleção: selecao_decl -> if ( expressao ) statement | if ( expressao ) statement else statement */
selecao_decl:
    IF OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement
    {
      /* Cria nó para seleção "if" sem "(", ")" */
      AST *node = newASTNodeLine("selecao_if", g_last_line);
      addChild(node, newASTNodeLine("IF", g_last_line));
      /* Removidos: addChild(node, newASTNodeLine("(", g_last_line));
         addChild(node, newASTNodeLine(")", g_last_line)); */
      addChild(node, $3); /* Adiciona a expressão */
      addChild(node, $5); /* Adiciona o statement */
      $$ = node;
    }
  | IF OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement ELSE statement
    {
      /* Cria nó para seleção "if-else" sem "(", ")" */
      AST *node = newASTNodeLine("selecao_if_else", g_last_line);
      addChild(node, newASTNodeLine("IF", g_last_line));
      /* Removidos: addChild(node, newASTNodeLine("(", g_last_line));
         addChild(node, newASTNodeLine(")", g_last_line)); */
      addChild(node, $3); /* Adiciona a expressão */
      addChild(node, $5); /* Adiciona o statement do if */
      addChild(node, newASTNodeLine("ELSE", g_last_line));
      addChild(node, $7); /* Adiciona o statement do else */
      $$ = node;
    }
    ;

/* Regra para iteração: iteracao_decl -> while ( expressao ) statement */
iteracao_decl:
    WHILE OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS statement
    {
      /* Cria nó para iteração "while" sem "(", ")" */
      AST *node = newASTNodeLine("iteracao_while", g_last_line);
      addChild(node, newASTNodeLine("WHILE", g_last_line));
      /* Removidos: addChild(node, newASTNodeLine("(", g_last_line));
         addChild(node, newASTNodeLine(")", g_last_line)); */
      addChild(node, $3); /* Adiciona a expressão */
      addChild(node, $5); /* Adiciona o statement */
      $$ = node;
    }
    ;

/* Regra para retorno: retorno_decl -> return ; | return expressao ; */
retorno_decl:
    RETURN SEMICOLON
    {
      /* Cria nó para retorno vazio */
      AST *node = newASTNodeLine("retorno_decl", g_last_line);
      addChild(node, newASTNodeLine("RETURN", g_last_line));
      /* Removido: addChild(node, newASTNodeLine(";", g_last_line)); */
      $$ = node;
    }
  | RETURN expressao SEMICOLON
    {
      /* Cria nó para retorno com expressão */
      AST *node = newASTNodeLine("retorno_decl", g_last_line);
      addChild(node, newASTNodeLine("RETURN", g_last_line));
      addChild(node, $2);
      /* Removido: addChild(node, newASTNodeLine(";", g_last_line)); */
      $$ = node;
    }
    ;

/* Regra para expressão: expressao -> var = expressão | simples_expressao */
expressao:
    var EQUAL expressao
    {
      /* Cria nó para atribuição */
      AST *node = newASTNodeLine("expressao_atribuicao", g_last_line);
      addChild(node, $1); /* Adiciona a variável */
      addChild(node, newASTNodeLine("=", g_last_line));
      addChild(node, $3); /* Adiciona a expressão */
      $$ = node;
    }
  | simples_expressao
    {
      /* Passa a expressão simples */
      $$ = $1;
    }
    ;

/* Regra para variável: var -> ID | ID [ expressão ] */
var:
    ID
    {
      /* Cria nó para variável simples */
      AST *node = newASTNodeLine("var", g_last_line);
      addChild(node, newASTNodeLine($1, g_last_line));
      free($1); /* Libera a string do ID */
      $$ = node;
    }
  | ID OPEN_SQUARE_BRACKETS expressao CLOSE_SQUARE_BRACKETS
    {
      /* Cria nó para variável com índice */
      AST *node = newASTNodeLine("var_array", g_last_line);
      addChild(node, newASTNodeLine($1, g_last_line));
      /* Removidos: addChild(node, newASTNodeLine("[", g_last_line));
         addChild(node, newASTNodeLine("]", g_last_line)); */
      addChild(node, $3); /* Adiciona a expressão de índice */
      free($1); /* Libera a string do ID */
      $$ = node;
    }
    ;

/* Regra para expressão simples: simples_expressao -> soma_expressao relacional soma_expressao | soma_expressao */
simples_expressao:
    soma_expressao relacional soma_expressao
    {
      /* Cria nó para expressão relacional */
      AST *node = newASTNodeLine("simples_expressao", g_last_line);
      addChild(node, $1); /* Adiciona a primeira soma_expressao */
      addChild(node, $2); /* Adiciona o operador relacional */
      addChild(node, $3); /* Adiciona a segunda soma_expressao */
      $$ = node;
    }
  | soma_expressao
    {
      /* Passa a soma_expressao */
      $$ = $1;
    }
    ;

/* Regra para operador relacional: relacional -> <= | < | > | >= | == | != */
relacional:
    MINOR_EQUAL
    {
      /* Cria nó para operador <= */
      $$ = newASTNodeLine("<=", g_last_line);
    }
  | MINOR
    {
      /* Cria nó para operador < */
      $$ = newASTNodeLine("<", g_last_line);
    }
  | GREATER
    {
      /* Cria nó para operador > */
      $$ = newASTNodeLine(">", g_last_line);
    }
  | GREATER_EQUAL
    {
      /* Cria nó para operador >= */
      $$ = newASTNodeLine(">=", g_last_line);
    }
  | EQUAL_EQUAL
    {
      /* Cria nó para operador == */
      $$ = newASTNodeLine("==", g_last_line);
    }
  | DIFFERENT
    {
      /* Cria nó para operador != */
      $$ = newASTNodeLine("!=", g_last_line);
    }
    ;

/* Regra para expressão de soma: soma_expressao -> soma_expressao soma termo | termo */
soma_expressao:
    soma_expressao soma termo
    {
      /* Cria nó para operação de soma ou subtração */
      AST *node = newASTNodeLine("soma_expressao", g_last_line);
      addChild(node, $1); /* Adiciona a primeira soma_expressao */
      addChild(node, $2); /* Adiciona o operador soma */
      addChild(node, $3); /* Adiciona o termo */
      $$ = node;
    }
  | termo
    {
      /* Passa o termo */
      $$ = $1;
    }
    ;

/* Regra para operadores de soma: soma -> + | - */
soma:
    PLUS
    {
      /* Cria nó para operador + */
      $$ = newASTNodeLine("+", g_last_line);
    }
  | MINUS
    {
      /* Cria nó para operador - */
      $$ = newASTNodeLine("-", g_last_line);
    }
    ;

/* Regra para termo: termo -> termo mult fator | fator */
termo:
    termo mult fator
    {
      /* Cria nó para operação de multiplicação ou divisão */
      AST *node = newASTNodeLine("termo", g_last_line);
      addChild(node, $1); /* Adiciona o primeiro termo */
      addChild(node, $2); /* Adiciona o operador multiplicação */
      addChild(node, $3); /* Adiciona o fator */
      $$ = node;
    }
  | fator
    {
      /* Passa o fator */
      $$ = $1;
    }
    ;

/* Regra para operadores de multiplicação: mult -> * | / */
mult:
    MULT
    {
      /* Cria nó para operador * */
      $$ = newASTNodeLine("*", g_last_line);
    }
  | DIV
    {
      /* Cria nó para operador / */
      $$ = newASTNodeLine("/", g_last_line);
    }
    ;

/* Regra para fator: fator -> ( expressão ) | var | ativacao | NUM */
fator:
    OPEN_PARENTHESIS expressao CLOSE_PARENTHESIS
    {
      /* Cria nó para expressão entre parênteses */
      AST *node = newASTNodeLine("fator(paren)", g_last_line);
      /* Removidos: addChild(node, newASTNodeLine("(", g_last_line));
         addChild(node, newASTNodeLine(")", g_last_line)); */
      addChild(node, $2); /* Adiciona a expressão */
      $$ = node;
    }
  | var
    {
      /* Cria nó para variável */
      AST *node = newASTNodeLine("fator(var)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | ativacao
    {
      /* Cria nó para ativação de função */
      AST *node = newASTNodeLine("fator(ativ)", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
  | NUM
    {
      /* Cria nó para número */
      char buf[64];
      sprintf(buf, "NUM:%d", $1);
      AST *node = newASTNodeLine(buf, g_last_line);
      /* Não adiciona filhos, pois é uma folha */
      $$ = node;
    }
    ;

/* Regra para ativação de função: ativacao -> ID ( args ) */
ativacao:
    ID OPEN_PARENTHESIS args CLOSE_PARENTHESIS
    {
      /* Cria nó para ativação de função */
      AST *node = newASTNodeLine("ativacao", g_last_line);
      addChild(node, newASTNodeLine($1, g_last_line));
      /* Removidos: addChild(node, newASTNodeLine("(", g_last_line));
         addChild(node, newASTNodeLine(")", g_last_line)); */
      addChild(node, $3); /* Adiciona os argumentos */
      free($1); /* Libera a string do ID */
      $$ = node;
    }
    ;

/* Regra para argumentos: args -> arg_lista | vazio */
args:
    arg_lista
    {
      /* Passa a lista de argumentos */
      $$ = $1;
    }
  | /* vazio */
    {
      /* Cria nó indicando que não há argumentos */
      AST *node = newASTNodeLine("args_vazio", g_last_line);
      $$ = node;
    }
    ;

/* Regra para lista de argumentos: arg_lista -> arg_lista , expressao | expressao */
arg_lista:
    arg_lista COMMA expressao
    {
      /* Cria nó para lista de argumentos e adiciona o novo argumento */
      AST *node = newASTNodeLine("arg_lista", g_last_line);
      addChild(node, $1);
      /* Removido: addChild(node, newASTNodeLine(",", g_last_line)); */
      addChild(node, $3);
      $$ = node;
    }
  | expressao
    {
      /* Cria nó para lista de argumentos com um único argumento */
      AST *node = newASTNodeLine("arg_lista", g_last_line);
      addChild(node, $1);
      $$ = node;
    }
    ;

%%

/* Função para captura de erros sintáticos */
void yyerror(const char *s) {
    fprintf(stderr, 
      "ERRO SINTATICO: \"%s\" INVALIDO [linha: %d], COLUNA %d\n",
      g_last_lexeme, g_last_line, g_last_col
    );
}
