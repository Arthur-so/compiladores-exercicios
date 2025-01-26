/* main.c */
#include <stdio.h>
#include <stdlib.h>
#include "lexeme.h"

/* O Bison gera yyparse() em parser.tab.c */
extern int yyparse();

/* Torna o lexer global (visível em lexer_adapter.c) */
Lexer *lexer = NULL;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    lexer = initialize_lexer(argv[1], 256);
    if (!lexer) {
        fprintf(stderr, "Falha ao inicializar lexer.\n");
        return 1;
    }

    int ret = yyparse();
    if (ret == 0) {
        printf("Parse concluído SEM erros sintáticos.\n");
    } else {
        printf("Houve erro(s) sintático(s)!\n");
    }

    destroy_lexer(lexer);
    return 0;
}
