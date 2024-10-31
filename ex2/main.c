#include <stdlib.h>
#include <stdio.h>
#include "funcs.h"
// ./main "./file_to_read.txt"

#define BUFFER_SIZE 8

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
    while (1) {
        printf("aso1\n");
        char c = get_next_char(fptr, buffer);
        printf("aso2\n");
        if (c == EOF) {
            printf("break\n");
            break;
        }
        printf("%c", c);
    }
    

    deallocate_buffer(buffer);

    fclose(fptr);
    return EXIT_SUCCESS;
}