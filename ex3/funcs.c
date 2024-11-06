#include "funcs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void fill_buffer(FILE *fptr, Buffer *buffer);

Buffer* allocate_buffer(int size) {
    Buffer* buffer = (Buffer*)malloc(sizeof(Buffer));
    if (buffer == NULL) {
        printf("Error when allocating memory for buffer struct.\n");
        exit(EXIT_FAILURE);
    }

    buffer->data = (char*)malloc(size * sizeof(char));
    if (buffer->data == NULL) {
        printf("Error when allocating memory for buffer data.\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    memset(buffer->data, 0, size * sizeof(char));  // Inicializa todos os bytes do buffer com 0

    buffer->next_char_id = 0;
    buffer->next_char_line = 0;
    buffer->size = size;

    return buffer;
}

void deallocate_buffer(Buffer* buffer) {
    if (buffer) {
        free(buffer->data);
        free(buffer);
    }
}

char get_next_char(FILE *fptr, Buffer* buffer) {
    if (buffer->next_char_id >= buffer->size || buffer->data[buffer->next_char_id] == '\0') {
        fill_buffer(fptr, buffer);
        if (buffer->data[0] == '\0') {  // Verifica fim de arquivo
            return EOF;
        }
    }
    if (buffer->data[buffer->next_char_id] == '\n') { // Nova linha
        buffer->next_char_line++;
    }
    return buffer->data[buffer->next_char_id++];
}

void unget_char(Buffer* buffer) {
    if (buffer->next_char_id > 0) {
        buffer->next_char_id--;
    }
}

void fill_buffer(FILE *fptr, Buffer *buffer) {
    memset(buffer->data, 0, buffer->size); // Reseya todos os bytes do buffer com 0
    if (fgets(buffer->data, buffer->size, fptr) != NULL) {
        buffer->next_char_id = 0;
    }
}

int is_char(char c) {
    int ascii_code = (int)c;

    if ((ascii_code >= 48 && ascii_code <= 57) || (ascii_code >= 65 && ascii_code <= 90))
        return 1;
    return 0;
}
