#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexeme.h"
#include "ast.h"
#include "symtab.h"
#include "semantic.h"

extern AST *g_root;    
extern int yyparse();

Lexer *lexer = NULL;

char g_last_lexeme[64];
int  g_last_line;
int  g_last_col;

/* Função para fazer somente a varredura léxica e imprimir tokens */
void doLexicalAnalysisOnly(Lexer *lexer) {
    while (1) {
        Token *t = getToken(lexer);
        
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

        // Imprime token
        printf("%d \"%s\" [linha: %d]\n",
               t->token_code, t->lexeme, t->line);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,"Uso: %s <arquivo> [opções]\n", argv[0]);
        return 1;
    }

    int only_lex = 0, print_ast = 0, print_symtab = 0;

    /* Verifica flags */
    for (int i=2; i<argc; i++) {
        if (!strcmp(argv[i],"-l") || !strcmp(argv[i],"-L")) only_lex = 1;
        else if (!strcmp(argv[i],"-p") || !strcmp(argv[i],"-P")) print_ast = 1;
        else if (!strcmp(argv[i],"-s") || !strcmp(argv[i],"-S")) print_symtab = 1;
    }

    lexer = initializeLexer(argv[1],256);
    if (!lexer) {
        fprintf(stderr,"Falha ao abrir arquivo: %s\n", argv[1]);
        return 1;
    }

    if (only_lex) {
        doLexicalAnalysisOnly(lexer);
        destroyLexer(lexer);
        return 0;
    }

    int ret = yyparse();
    if (ret == 0) {
        // Parsing OK
        if (g_root) {
            /* 1) Monta a Tabela de Símbolos */
            buildSymtab(g_root);
            /* 2) Verifica semântica */
            checkSemantics(g_root);

            /* 3) Se for para printar AST */
            if (print_ast) {
                printAST(g_root, 0);
            }
            /* 4) Se for para printar Tabela de Símbolos */
            if (print_symtab) {
                printSymTab();
            }
        }
    } 
    destroyLexer(lexer);
    return 0;
}
