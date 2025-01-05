#include <stdlib.h>
#include <stdio.h>
#include "funcs.h"
#include "lexeme.h"
#include <ctype.h>
// ./main "./lexemas.c"

#define BUFFER_SIZE 256
#define HASH_SIZE 11

int get_tipo(char ch) {
    if (isalpha(ch)) {
        return 0; // Letra
    } else if (isdigit(ch)) {
        return 1; // Dígito
    } else {
        switch (ch) {
            case '+': return 2;
            case '-': return 3;
            case '*': return 4;
            case ';': return 5;
            case ',': return 6;
            case '(': return 7;
            case ')': return 8;
            case '[': return 9;
            case ']': return 10;
            case '{': return 11;
            case '}': return 12;
            case '>': return 13;
            case '<': return 14;
            case '=': return 15;
            case '!': return 16;
            case '/': return 17;
            default:
                if (isspace(ch) || ch == EOF) {
                    return 18; // Branco
                } else {
                    return 19; // Outro
                }
        }
    }
}

typedef struct HashNode {
    char key[10];
    int value;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* head;
} HashTable[HASH_SIZE];

unsigned int hash(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * 31) + *str++;
    }
    return hash % HASH_SIZE;
}

void addToHashTable(HashTable hashTable, const char* key, int value) {
    unsigned int index = hash(key);

    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    strcpy(newNode->key, key);
    newNode->value = value;
    newNode->next = hashTable[index].head; // Insere no início da lista encadeada
    hashTable[index].head = newNode;
}

void initializeHashTable(HashTable hashTable) {
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i].head = NULL; // Inicializa com listas vazias
    }

    // Adiciona palavras-chave
    addToHashTable(hashTable, "else", 0);
    addToHashTable(hashTable, "if", 1);
    addToHashTable(hashTable, "int", 2);
    addToHashTable(hashTable, "return", 3);
    addToHashTable(hashTable, "void", 4);
    addToHashTable(hashTable, "while", 5);
}



int findInHashTable(HashTable hashTable, const char* key) {
    unsigned int index = hash(key);

    HashNode* current = hashTable[index].head;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value; // Palavra encontrada
        }
        current = current->next;
    }
    return 10; // Palavra não encontrada
}

int classifica_lexema(char* lexema, int estado, HashTable hashTable) {
    int token = estado;
    if (estado == 10) {
        token = findInHashTable(hashTable, lexema);
    }
    printf("lexema: %s - token: %d\n", lexema, token);
    return token;
}

int main(int argc, char **argv) {
     HashTable hashTable;
    initializeHashTable(hashTable);

    // Teste de busca
    char* testWords[] = {"else", "if", "int", "return", "void", "while", "unknown"};
    for (int i = 0; i < 7; i++) {
        printf("Palavra: %s -> Token: %d\n", testWords[i], findInHashTable(hashTable, testWords[i]));
    }
    printf("COMECANDOOOO\n");

    int Aceita[32] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1,
        1, 1, 1, 1, 1, 1, 1,
        1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    int AdicionaAoToken[20] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 0, 0
    };
    int AdicionaAoTokenEstado[32] = {
        1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
        1, 1,
        1, 1, 1, 1, 1, 1, 1,
        1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    int T[32][20] = {
        {1, 2, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 3, 4, 5, 6, 7, 0, -1},
        {1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        {11, 2, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
        {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 12, 12, 12, 12},
        {14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 14, 14, 14, 14},
        {16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 18, -1, -1, -1, -1},
        {31, 31, 31, 31, 8, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31},
        {8, 8, 8, 8, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
        {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 19, 8, 8},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 10
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 11
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 12
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 13
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 14
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 15
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 16
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 17
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 18
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 19
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 20
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 21
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 22
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 23
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 24
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 25
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 26
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 27
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 28
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 29
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Estado 30
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  // Estado 31
    };


    int Avance[32][20] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

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
    Classification classifier;
    char aux_lexeme[64];
    int char_lexeme_id = 0;
    enum Token token;
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