#include <stdlib.h>
#include <stdio.h>
#include "funcs.h"
#include "lexeme.h"
// ./main "./lexemas.c"

#define BUFFER_SIZE 256

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Wrong args passed!\n");
        return 0;
    }

    char* filename = argv[1];

    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("Couldn't open the file: %s\n", filename);
        return EXIT_FAILURE;
    }

    Buffer* buffer = allocate_buffer(BUFFER_SIZE);
    Classification classifier;
    char aux_lexeme[64];
    int char_lexeme_id = 0;
    enum Token token;
    int is_chain_char = 0;
    int is_chain_number = 0; 
    while (1) {
        char c = get_next_char(fptr, buffer);
        if (c == EOF) {
            break;
        }

        // verifica se e' um char a ser inserido numa cadeia de caracteres: e' o primeiro caracter ou o ultimo tambem e' um caracter
        is_chain_char = (is_char(c)
                    && (char_lexeme_id == 0 || is_char(aux_lexeme[char_lexeme_id - 1])));
        // verifica se e' um numero a ser inserido numa cadeia de numeros: e' o primeiro algarismo ou o ultimo tambem e' um numero
        // is_chain_number = (is_number(c)
        //             && (char_lexeme_id == 0 || is_number(aux_lexeme[char_lexeme_id])));
        // adiciona a cadeia de numeros ou de caracteres
        if (is_chain_char || is_chain_number) {
            aux_lexeme[char_lexeme_id++] = c;
            buffer->next_char_id++;
        }
        else {
            // caso seja o primeiro valor lido para montagem do lexema, avanco no buffer
            if (char_lexeme_id == 0) {
                buffer->next_char_id++;
            }
            // caso seja nao primeiro valor lido para montagem do lexema, apenas indica o final de um lexema (nao consome char do buffer)
            else {
                aux_lexeme[char_lexeme_id] = '\0';
                token = classifyLexeme(aux_lexeme);

                classifier.line = buffer->next_char_line;
                classifier.token = token;
                strcpy(classifier.lexeme, aux_lexeme);

                char_lexeme_id = 0;

                printf("Lexeme: %s - line: %d - token: %d\n", classifier.lexeme, classifier.line, classifier.token);
            }
        }
    }

    deallocate_buffer(buffer);

    fclose(fptr);
    return EXIT_SUCCESS;
}