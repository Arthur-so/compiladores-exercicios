#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexeme.h"
#include "ast.h"  /* se você usa AST */
extern AST *g_root;    /* definido em parser.y */

extern int yyparse();  /* gerado pelo Bison */
Lexer *lexer = NULL;
/* 
   Se você está guardando em yylex() o último lexema para erro sintático, 
   pode querer extern as variáveis do parser.y, ex.:
   extern char g_last_lexeme[64];
   extern int g_last_line;
   extern int g_last_col;
*/

/* Função para fazer somente a varredura léxica e imprimir tokens */
void doLexicalAnalysisOnly(Lexer *lexer) {
    while (1) {
        Token *t = get_token(lexer);
        
        if (t->done) {
            // Fim de arquivo
            break;
        }

        if (t->error) {
            // Erro léxico
            fprintf(stderr,
                "ERRO LEXICO: \"%s\" INVALIDO [linha: %d], COLUNA %d\n",
                t->lexeme, t->line, t->column
            );
            // Se quiser parar ao primeiro erro, faça: break;
            break;
        }

        if (t->token_code == L_COMMENT) {
            // Se for comentário, ignore
            continue;
        }

        // Imprime no formato: <TOKEN> "<lexema>" [linha: X]
        // Precisamos mapear token_code -> nome do token
        // Ou, se você só quer imprimir o "código" mesmo, tudo bem.
        // Exemplo simples:
        printf("%d \"%s\" [linha: %d]\n",
               t->token_code, t->lexeme, t->line);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo> [opções]\n", argv[0]);
        fprintf(stderr, "Opções:\n");
        fprintf(stderr, "  -l ou -L  => modo apenas léxico\n");
        fprintf(stderr, "  -p ou -P  => modo sintático + print da AST\n");
        return 1;
    }

    int only_lex = 0;
    int print_ast = 0;

    /* Varre os argumentos */
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "-L") == 0) {
            only_lex = 1;
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "-P") == 0) {
            print_ast = 1;
        }
    }

    /* Inicializa o lexer */
    lexer = initialize_lexer(argv[1], 256);
    if (!lexer) {
        fprintf(stderr, "Falha ao abrir arquivo: %s\n", argv[1]);
        return 1;
    }

    /* Se for apenas léxico, chamamos doLexicalAnalysisOnly */
    if (only_lex) {
        doLexicalAnalysisOnly(lexer);
        destroy_lexer(lexer);
        return 0;
    }

    /* Caso contrário, chamamos o parser (que invoca yylex()) */
    int ret = yyparse();
    if (ret == 0) {
        // Nenhum erro sintático
        if (print_ast && g_root) {
            // Imprime AST
            printAST(g_root);
        }
    } else {
        printf("Houve erro(s) sintático(s).\n");
    }

    destroy_lexer(lexer);
    return 0;
}
