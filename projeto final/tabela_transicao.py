T = [
    #    0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16    17    18   19
    #    L     D     +     -     *     ;     ,     (     )     [     ]    {      }     >    <     =     !     /     B   Outro
    [   1,    2,   20,   21,   22,   23,   24,   25,    26,   27,   28,   29,   30,    3,    4,    5,    6,    7,    0,   -1], # Estado 0

    [    1, [10], [10], [10], [10], [10], [10], [10], [10], [10], [10], [10], [10], [10], [10], [10], [10], [10], [10], [10]], # Estado 1 (letras)
    [ [11],    2, [11], [11], [11], [11], [11], [11], [11], [11], [11], [11], [11], [11], [11], [11], [11], [11], [11], [11]], # Estado 2 (digitos)

    [ [12], [12], [12], [12], [12], [12], [12], [12], [12], [12], [12], [12], [12], [12], [12],   13, [12], [12], [12], [12]], # Estado 3 (>x)
    [ [14], [14], [14], [14], [14], [14], [14], [14], [14], [14], [14], [14], [14], [14], [14],   15, [14], [14], [14], [14]], # Estado 4 (<x)
    [ [16], [16], [16], [16], [16], [16], [16], [16], [16], [16], [16], [16], [16], [16], [16],   17, [16], [16], [16], [16]], # Estado 5 (=x)
    [   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   18,   -1,   -1,   -1,   -1], # Estado 6 (!x)

    [ [31], [31], [31], [31],    8, [31], [31], [31], [31], [31], [31], [31], [31], [31], [31], [31], [31], [31], [31], [31]], # Estado 7 (/x)
    [    8,    8,    8,    8,    9,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8], # Estado 8 (/*xxx)
    [    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,   19,    8,    8], # Estado 9 (/*xxx*xxx)


    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 10 Aceitacao (ID)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 11 Aceitacao (NUM)

    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 12 Aceitacao (>)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 13 Aceitacao (>=)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 14 Aceitacao (<)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 15 Aceitacao (<=)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 16 Aceitacao (=)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 17 Aceitacao (==)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 18 Aceitacao (!=)

    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 19 Aceitacao (/*xxx*/)

    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 20 Aceitacao (+)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 21 Aceitacao (-)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 22 Aceitacao (*)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 23 Aceitacao (;)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 24 Aceitacao (,)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 25 Aceitacao (()
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 26 Aceitacao ())
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 27 Aceitacao ([)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 28 Aceitacao (])
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 29 Aceitacao ({)
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 30 Aceitacao (})
    [  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0],  [0]], # Estado 31 Aceitacao (/)

]

Aceita = [False, False, False, False, False, False, False, False, False, False,
          True, True,
          True, True, True, True, True, True, True,
          True,
          True, True, True, True, True, True, True, True, True, True, True, True]

AdicionaAoToken = [True, True, True, True, True, True, True, True, True, True, True, True, True, True, True, True, True, True, False, False]

AdicionaAoTokenEstado = [True, True, True, True, True, True, True, True, False, False,
                         True, True,
                         True, True, True, True, True, True, True,
                         True,
                         True, True, True, True, True, True, True, True, True, True, True, True]

palavras_chave = {
    "if": 101,
    "else": 102,
    "int": 103,
    "return": 104,
    "void": 105,
    "while": 106,
}

def classifica_token(token, estado):
    id = estado
    if estado == 10:
        if token in palavras_chave:
            id = palavras_chave[token]
    elif estado == 19:
        return
    print(f"{token} - {id}")
            

def get_tipo(ch):
    if ch.isalpha():
        return 0  # Letra
    elif ch.isdigit():
        return 1  # Dígito
    elif ch in "+-*;,()[]{}><=!/":
        return {'+': 2, '-': 3, '*': 4, ';': 5, ',': 6, '(': 7, ')': 8, '[': 9, ']': 10, '{': 11, '}': 12, '>': 13, '<': 14, '=': 15, '!': 16, '/': 17}[ch]
    elif ch.isspace():
        return 18  # Branco
    else:
        return 19  # Outro

def avance(estado):
    if isinstance(estado, list):
        return False
    return True

def analisador_lexico(codigo):
    estado = 0
    i = 0
    token = ''
    while i < len(codigo):
        if estado == -1:
            print("Erro!!!!!")
            break

        if Aceita[estado]:
            #print(f"{token} - estado: {estado}")
            classifica_token(token, estado)
            token = ''

        char_code = get_tipo(codigo[i])
        novo_estado = T[estado][char_code]
        
        if avance(novo_estado):
            if AdicionaAoToken[char_code] and AdicionaAoTokenEstado[novo_estado]:
                token += codigo[i]
            i += 1
        else:
            novo_estado = novo_estado[0]
        estado = novo_estado
    return estado

codigo = """
/* Um programa para calcular o mdc
   segundo o algoritmo de Euclides. */

int gcd (int u, int v)
{ if (v == 0) return u ;
  else return gcd(v,u-u/v*v);
  /* u-u/v*v == u mod v */
}

void main(void)
{ int x; int y;
  x = input(); y = input();
  output(gcd(x,y));
}
"""

if analisador_lexico(codigo) == 0:
    print("Aprovado")
else:
    print("Reprovado")