#include "funcs.h"
#include <stdlib.h>
#include <stdio.h>

Buffer * allocate_buffer(int buffer_size) {
    Buffer *buffer;
    buffer = (Buffer *)malloc(sizeof(Buffer));
    if (buffer == NULL) {
        printf("Error when alocating memory struct.");
        exit(EXIT_FAILURE);
    }

    buffer->buffer_ptr = (char *)malloc(buffer_size * sizeof(char));
    if (buffer == NULL) {
        printf("Error when alocating memory buffer.");
        exit(EXIT_FAILURE);
    }
    buffer->buffer_size = buffer_size;
    return buffer;
}

void deallocate_buffer(Buffer* buffer) {
    if (buffer) {
        free(buffer->buffer_ptr);
        free(buffer);
    }
}

void replace_print(FILE* fptr, Buffer* buffer) {
    char ch;
    int i = 0;
    buffer->buffer_ptr[buffer->buffer_size - 1] = '\0'; // Adiciona terminador no final do buffer
    while ((ch = fgetc(fptr)) != EOF) {

        // Chegou na última posição do buffer
        if (buffer->buffer_size - 1 == i) {
            printf("%s\n", buffer->buffer_ptr);
            i = 0;
        }

        if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u') {
            buffer->buffer_ptr[i] = ch - 'a' + 'A'; // Converte vogais minúsculas para maiúsculas
        }
        else if (ch == 'A' || ch == 'E' || ch == 'I' || ch == 'O' || ch == 'U') {
                buffer->buffer_ptr[i] = ch - 'A' + 'a'; // Converte vogais minúsculas para maiúsculas
        }
        else {
                buffer->buffer_ptr[i] = ch;
        }
        i++;
    }

    if (i != 0) {
        buffer->buffer_ptr[i] = '\0';
        printf("%s", buffer->buffer_ptr);
    }
}