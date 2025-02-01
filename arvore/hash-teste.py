# Definição do tamanho da tabela hash
HASH_SIZE = 31

# Lista de palavras-chave e seus respectivos tokens
keywords = {
    "else": 100,
    "if": 101,
    "int": 102,
    "return": 103,
    "void": 104,
    "while": 105,
}

# Função hash usada na implementação C
def hash_keyword(word):
    hash_value = 0
    for char in word:
        hash_value = (hash_value * 31) + ord(char)
    return hash_value % HASH_SIZE

# Criando a tabela hash simulada
keyword_table = {i: [] for i in range(HASH_SIZE)}

# Inserindo palavras-chave na tabela
for keyword, token in keywords.items():
    index = hash_keyword(keyword)
    keyword_table[index].append((keyword, token))

# Testando a busca na tabela hash
def lookup_keyword(lexeme):
    index = hash_keyword(lexeme)
    for key, token in keyword_table[index]:
        if key == lexeme:
            return token
    return None  # Retorna None se não encontrar (equivalente a L_ID)

# Testando todas as palavras-chave
print("\n### Testando Classificação das Palavras-Chave ###")
for kw in keywords.keys():
    token_found = lookup_keyword(kw)
    print(f"Palavra-chave: {kw:7} → Token encontrado: {token_found}")
