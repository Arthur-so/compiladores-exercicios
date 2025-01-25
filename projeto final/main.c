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
    int count = 0;
    while (1) {
        token = get_token(lexer);
        if (token->error) {
            printf("deu erro");
            break;
        }
        printf("%s : %d\n", token->lexeme, token->token_id);
        if (token->done) {
            printf("acabou");
            break;
        }

        // if (count == 20)
        //     break;
        // count++;
    }
    return 0;
}