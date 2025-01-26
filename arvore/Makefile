# Makefile

CC=gcc
BISON=bison

all: compilador

parser.tab.c parser.tab.h: parser.y
	$(BISON) -d parser.y

compilador: parser.tab.o lexer.o lexer_adapter.o ast.o main.o
	$(CC) -o compilador parser.tab.o lexer.o lexer_adapter.o ast.o main.o

parser.tab.o: parser.tab.c
	$(CC) -c parser.tab.c

lexer.o: lexer.c lexeme.h
	$(CC) -c lexer.c

lexer_adapter.o: lexer_adapter.c parser.tab.h lexeme.h
	$(CC) -c lexer_adapter.c

ast.o: ast.c ast.h
	$(CC) -c ast.c

main.o: main.c lexeme.h ast.h
	$(CC) -c main.c

clean:
	rm -f *.o parser.tab.* compilador
