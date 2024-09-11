# Instruções de Execução

Este programa converte uma gramática livre de contexto (GLC) em sua forma normal de Chomsky (FNC). A entrada é um arquivo de texto contendo a definição de uma gramática, incluindo símbolos terminais, não-terminais, produções e o símbolo inicial. O resultado é uma gramática equivalente, mas com as produções na Forma Normal de Chomsky, que será gravada em um arquivo de saída.

## Como Executar

### 1. Compilação
Para compilar o código, você pode usar um compilador como o `g++`. Abra um terminal ou prompt de comando e execute o seguinte comando:

```bash
g++ -o cnf_converter cnf_converter.cpp
```

### 2. Execução
Para executar o programa, utilize o seguinte comando:

```bash
./chomsky_mod <arquivo_entrada.txt> <arquivo_saida.txt>
```
<arquivo_entrada.txt>: Nome do arquivo de entrada que contém a gramática.
<arquivo_saida.txt>: Nome do arquivo de saída onde a gramática convertida será salva.

### 3. Exemplos de uso
```bash
./cnf_converter entrada.txt saida.txt
```

### 4. Formato do arquivo de entrada
O arquivo de entrada deve seguir o seguinte formato:

* Linha 1: Número total de símbolos terminais.
* Linha 2: Lista de símbolos terminais separados por vírgulas.
* Linha 3: Número total de símbolos não-terminais.
* Linha 4: Lista de símbolos não-terminais separados por vírgulas.
* Linha 5: Símbolo inicial.
* Linha 6: Número total de produções.
* Linhas seguintes: Produções no formato LHS -> RHS, onde LHS é o lado esquerdo da produção e RHS pode conter terminais, não-terminais ou alternativas separadas por "|".

### 5. Formato do arquivo de saída
O arquivo de saída seguirá o mesmo formato do arquivo de entrada, mas a gramática estará na Forma Normal de Chomsky (FNC).

### 6. Exemplos de entrada e saída
No repositório encontra-se dois exemplos de entrada e saída 

[Entrada 1](./entrada1.txt)
[Entrada 2](./entrada2.txt)
[Saída 1](./saida1.txt)
[saida 2](./saida2.txt)
