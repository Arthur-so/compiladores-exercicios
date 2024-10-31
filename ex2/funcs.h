#include <stdio.h>

typedef struct {
    char* data;
    int next_char_id;
    int next_char_line;
    int size;
} Buffer;

Buffer* allocate_buffer(int size);
void deallocate_buffer(Buffer* buffer);
char get_next_char(FILE *fptr, Buffer* buffer);

