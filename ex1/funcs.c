#include "funcs.h"
#include <stdlib.h>
#include <stdio.h>

Buffer * allocate_buffer(int size) {
    Buffer *buffer;
    buffer = (Buffer *)malloc(sizeof(Buffer));
    if (buffer == NULL) {
        printf("Error when alocating memory struct.");
        exit(EXIT_FAILURE);
    }

    buffer->data = (char *)malloc(size * sizeof(char));
    if (buffer == NULL) {
        printf("Error when alocating memory buffer.");
        exit(EXIT_FAILURE);
    }
    buffer->size = size;
    return buffer;
}

void deallocate_buffer(Buffer* buffer) {
    if (buffer) {
        free(buffer->data);
        free(buffer);
    }
}

void replace_print(FILE* fptr, Buffer* buffer) {
    char ch;
    int i = 0;
    buffer->data[buffer->size - 1] = '\0'; // Adiciona terminador no final do buffer
    while ((ch = fgetc(fptr)) != EOF) {

        // Chegou na última posição do buffer
        if (buffer->size - 1 == i) {
            printf("%s\n", buffer->data);
            i = 0;
        }

        if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u') {
            buffer->data[i] = ch - 'a' + 'A'; // Converte vogais minúsculas para maiúsculas
        }
        else if (ch == 'A' || ch == 'E' || ch == 'I' || ch == 'O' || ch == 'U') {
                buffer->data[i] = ch - 'A' + 'a'; // Converte vogais minúsculas para maiúsculas
        }
        else {
                buffer->data[i] = ch;
        }
        i++;
    }

    if (i != 0) {
        buffer->data[i] = '\0';
        printf("%s", buffer->data);
    }
}