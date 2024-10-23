#include <stdlib.h>
#include <stdio.h>
#include "funcs.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Wrong args passed!\n");
        return 0;
    }

    char* filename = argv[1];
    int buffer_size = atoi(argv[2]);

    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("Couldn't open the file: %s\n", filename);
        return EXIT_FAILURE;
    }

    Buffer* buffer = allocate_buffer(buffer_size);

    replace_print(fptr, buffer);

    deallocate_buffer(buffer);

    fclose(fptr);
    return EXIT_SUCCESS;
}