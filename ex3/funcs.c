#include "funcs.h"

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
    buffer->next_char_line = 1;
    buffer->size = size;
    buffer->new_line = 0;

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
        if (buffer->new_line) {
            buffer->next_char_line++;
            buffer->new_line = 0;
        }
    }
    if (buffer->data[buffer->next_char_id] == '\n') { // Nova linha
        buffer->new_line = 1;
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
        buffer->next_char_id = 1;
    }
}