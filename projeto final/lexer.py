
T = [
    #   0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18  19
    #   L    D    +    -    *    ;    ,    (    )    [    ]   {     }    >   <    =    !    /    B  Outro
    [  1,   2,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,    3,   3,   3,   4,   5,   0,  -1], # Estado 0
    [   1, [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8]], # Estado 1 (letras)
    [ [8],   2, [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8]], # Estado 2 (digitos)
    [ [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8],   8, [8], [8], [8], [8]], # Estado 3 (>,<,=,>=,<=ou==)
    [  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   8,  -1,  -1,  -1,  -1], # Estado 4 (!=)
    [ [8], [8], [8], [8],   6, [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8], [8]], # Estado 5 (/*)
    [   6,   6,   6,   6,   7,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6], # EStado 6 (/*comentario*)
    [   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   8,   6,   6], # EStado 7 (/*comentario*/)
    [   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0], # Estado 8 Aceitacao
]

Aceita = [False, False, False, False, False, False, False, False, True]
AdicionaAoToken = [True, True, True, True, True, True, True, True, True, True, True, True, True, True, True, True, True, False, False]


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
            print(f"Token processado:{token}- tamanho: {len(token)}")
            token = ''

        char_code = get_tipo(codigo[i])
        novo_estado = T[estado][char_code]
        
        if avance(novo_estado):
            if AdicionaAoToken[char_code]:
                token += codigo[i]
            i += 1
        else:
            novo_estado = novo_estado[0]
        estado = novo_estado

codigo = """
int xk = 10;
if (x < 20) {
    return x;
}
/* Comentário */
"""

analisador_lexico(codigo)
