#include <stdlib.h>
#include <stdio.h>
#include "funcs.h"
#include <ctype.h>
// ./main "./lexemas.c"

#define BUFFER_SIZE 256
#define HASH_SIZE 11

int main(int argc, char **argv) {
    HashTable hashTable;
    initializeHashTable(hashTable);

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
    char aux_lexeme[64];
    int char_lexeme_id = 0;
    int is_chain_char = 0;
    int is_chain_number = 0; 

    int estado = 0;
    int char_code = 0;
    int novo_estado = 0;
    char c;
    int avance = 0;
    while (1) {
        if (c == EOF) {
            printf("saindo: %c\n", c);
            break;
        }
        if (estado == -1) {
            printf("DEU ERRO");
            break;
        }
        if (Aceita[estado]){
            aux_lexeme[char_lexeme_id] = '\0'; // Finaliza o lexema
            classifica_lexema(aux_lexeme, estado, hashTable);
            //printf("%s - estado: %d\n", aux_lexeme, estado);
            char_lexeme_id = 0;
        }

        c = get_next_char(fptr, buffer);
        char_code = get_tipo(c);
        
        novo_estado = T[estado][char_code];
        avance = Avance[estado][char_code];
        
        if (avance) {
            if (AdicionaAoToken[char_code] && AdicionaAoTokenEstado[novo_estado]) {
                aux_lexeme[char_lexeme_id++] = c;
            }
            buffer->next_char_id++;
        }
        estado = novo_estado; 
    }
    if (estado == 0 || Aceita[estado]) {
        printf("Aprovado\n");
    }
    else {
        printf("Reprovado - estado: %d - c: %c\n", estado, c);

    }

    deallocate_buffer(buffer);

    fclose(fptr);
    return EXIT_SUCCESS;
}