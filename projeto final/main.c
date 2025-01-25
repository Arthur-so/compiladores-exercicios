#include <stdlib.h>
#include <stdio.h>
#include "lexeme.h"
// ./main "./lexemas.c"

#define BUFFER_SIZE 256

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Wrong args passed!\n");
        return 0;
    }

    int print_token = 0;
    if (argc == 3 && (strcmp(argv[2], "-l") == 0 || strcmp(argv[2], "-L") == 0)) {
        print_token = 1;
    }

    char* filename = argv[1];
    Lexer* lexer = initialize_lexer(filename, BUFFER_SIZE);
    Token* token;
    int count = 0;
    while (1) {
        token = get_token(lexer);
        
        if (token->error) {
            printf("ERRO LEXICO: \"%s\" INVALIDO [linha: %d], COLUNA %d\n", token->lexeme, token->line, token->column);
            break;
        }

        if (token->done) {
            break;
        }

        if (token->token_id == COMMENT)
            continue;

        if (print_token) {
            printf("%d \"%s\" [linha: %d]\n", token->token_id, token->lexeme, token->line);
        }
    }
    return 0;
}