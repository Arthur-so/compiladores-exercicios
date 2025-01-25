#include <stdlib.h>
#include <stdio.h>
#include "lexeme.h"
// ./main "./lexemas.c"

#define BUFFER_SIZE 256

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Wrong args passed!\n");
        return 0;
    }
    char* filename = argv[1];
    Lexer* lexer = initialize_lexer(filename, BUFFER_SIZE);
    Token* token;
    while (1) {
        token = get_token(lexer);
        printf("%s : %d", token->lexeme, token->token_id);
    }
    return EXIT_SUCCESS;
}