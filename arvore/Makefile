# Makefile

CC     = gcc
BISON  = bison
CFLAGS = -Wall -Wextra -g

all: compilador

parser.tab.c parser.tab.h: parser.y
	$(BISON) -d parser.y

compilador: parser.tab.o lexer.o lexer_interface.o ast.o main.o symtab.o semantic.o
	$(CC) $(CFLAGS) -o compilador parser.tab.o lexer.o lexer_interface.o ast.o main.o symtab.o semantic.o

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

lexer.o: lexer.c lexeme.h
	$(CC) $(CFLAGS) -c lexer.c

lexer_interface.o: lexer_interface.c parser.tab.h lexeme.h
	$(CC) $(CFLAGS) -c lexer_interface.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

main.o: main.c lexeme.h ast.h symtab.h semantic.h
	$(CC) $(CFLAGS) -c main.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

semantic.o: semantic.c symtab.h ast.h
	$(CC) $(CFLAGS) -c semantic.c

clean:
	rm -f *.o parser.tab.* compilador
