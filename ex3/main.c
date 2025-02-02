#include <stdlib.h>
#include <stdio.h>
#include "funcs.h"
#include "lexeme.h"
// ./main "./file_to_read.txt"

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
    int char_lexeme_id;
    enum Token token;
    while (1) {
        char c = get_next_char(fptr, buffer);
        if (c == EOF) {
            break;
        }
        if (is_char(c)) {
            aux_lexeme[char_lexeme_id++] = c;
        }
        else {
            if (char_lexeme_id != 0) {
                aux_lexeme[char_lexeme_id] = '\0';
                token = classifyLexeme(aux_lexeme);

                classifier.line = buffer->next_char_line;
                classifier.token = token;
                strcpy(classifier.lexeme, aux_lexeme);

                char_lexeme_id = 0;

                printf("Lexeme: %s - line: %d - token: %d\n", classifier.lexeme, classifier.line, classifier.token);
            }
        }

        //printf("%c", c);
    }

    deallocate_buffer(buffer);

    fclose(fptr);
    return EXIT_SUCCESS;
}