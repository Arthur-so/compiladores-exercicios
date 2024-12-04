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

        // Verifica se o caractere é parte de um lexema de letras
        is_chain_char = (is_char(c)
                    && (char_lexeme_id == 0 || is_char(aux_lexeme[char_lexeme_id - 1])));
        // Verifica se o caractere é parte de um número
        is_chain_number = (is_num(c)
                    && (char_lexeme_id == 0 || is_num(aux_lexeme[char_lexeme_id - 1])));
        // Adiciona o caractere ao lexema
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
                aux_lexeme[char_lexeme_id] = '\0'; // Finaliza o lexema
                token = classifyLexeme(aux_lexeme);

                classifier.line = buffer->next_char_line;
                classifier.token = token;
                strcpy(classifier.lexeme, aux_lexeme);

                // Reseta o lexema para o próximo
                char_lexeme_id = 0;

                printf("Lexeme: %s - line: %d - token: %d\n", classifier.lexeme, classifier.line, classifier.token);
            }
        }
    }

    deallocate_buffer(buffer);

    fclose(fptr);
    return EXIT_SUCCESS;
}