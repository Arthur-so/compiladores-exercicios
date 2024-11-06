#include <stdio.h>

enum Token {
    ID, INT, CHAR, FLOAT, DOUBLE, LONG, AUTO, BREAK, CASE, CONTINUE, DEFAULT,
    DO, ENUM, EXTERN, FOR, GOTO, IF, REGISTER, RETURN, SHORT, SIGNED,
    SIZEOF, STATIC, STRUCT, SWITCH, WHILE
};

typedef struct {
    const char *word;
    enum Token token;
} KeyWord;

extern KeyWord key_words[];

typedef struct {
    char* data;
    int next_char_id;
    int next_char_line;
    int size;
} Buffer;

typedef struct {
    int line;
    char lexema[64];
    enum Token token;
} Classification;

Buffer* allocate_buffer(int size);
void deallocate_buffer(Buffer* buffer);
char get_next_char(FILE *fptr, Buffer* buffer);
void unget_char(Buffer* buffer);
int is_char(char c); 
