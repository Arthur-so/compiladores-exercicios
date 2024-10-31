#include <stdio.h>

typedef struct {
    char* data;
    int size;
} Buffer;

Buffer* allocate_buffer(int size);
void deallocate_buffer(Buffer* buffer);
void replace_print(FILE* fptr, Buffer* buffer);

