#include "funcs.h"
#include <stdlib.h>
#include <stdio.h>

void fill_buffer(FILE *fptr, Buffer *buffer);

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
    fill_buffer(fptr, buffer);
    return buffer->data[buffer->next_char_id++];
}

void fill_buffer(FILE *fptr, Buffer *buffer) {
    printf("%d", buffer->next_char_id);
    if (buffer->data[buffer->next_char_id] == '\0') {
        fgets(buffer->data, buffer->size, fptr);
        buffer->next_char_id = 0;
    }
}
