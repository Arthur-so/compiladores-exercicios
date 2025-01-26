bison -d parser.y
# Isso gera parser.tab.c e parser.tab.h

gcc -c parser.tab.c         # gera parser.tab.o
gcc -c lexer.c              # gera lexer.o
gcc -c lexer_adapter.c      # gera lexer_adapter.o
gcc -c main.c               # gera main.o

# Finalmente linkamos tudo:
gcc -o compilador parser.tab.o lexer.o lexer_adapter.o main.o
