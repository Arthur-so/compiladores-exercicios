#include <stdio.h>

typedef struct {
    char* buffer_ptr;
    int buffer_size;
} Buffer;

Buffer* allocate_buffer(int buffer_size);
void deallocate_buffer(Buffer* buffer);
void replace_print(FILE* fptr, Buffer* buffer);

