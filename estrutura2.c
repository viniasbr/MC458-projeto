//Vamos fazer a estrutura de dados baseada em árvores binárias de busca balanceadas AVL
//vai ter uma árvore com cada linha que tem um elemento na matriz que estamos representando na árvore
//cada nó dessa árvore vai ter uma árvore AVL que representa os elementos daquela linha

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct No_linha_AVL {
    int linha;
    struct No_linha_AVL *esquerda;
    struct No_linha_AVL *direita;
    int altura;//daqui pra cima é coisa padrão de árvore AVL

    struct No_coluna_AVL *colunas; //esse vai ser o ponteiro para a árvore de colunas correspondente
} No_linha_AVL;

typedef struct No_coluna_AVL{
    int valor;
    int coluna;
    int altura;
    struct No_coluna_AVL *direita;
    struct No_coluna_AVL *esquerda;
} No_coluna_AVL;

//a ideia é que as linhas sejam um ponto de acesso para as colunas, que vão guardar os valores
//cada nó de linha tem uma árvore de colunas, mas cada nó de coluna só tem valor e posição na coluna
//acho que assim dá pra fazer tudo que é proposto nesse projeto do Santiago

// Funções auxiliares 
No_linha_AVL* criar_no_linha(int linha) {
    No_linha_AVL* novo_no = (No_linha_AVL*)malloc(sizeof(No_linha_AVL));
    novo_no->linha = linha;
    novo_no->esquerda = NULL;
    novo_no->direita = NULL;
    novo_no->colunas = NULL;
    novo_no->altura = 1;
    return novo_no;
}

No_coluna_AVL* criar_no_coluna(int coluna, int valor) {
    No_coluna_AVL* novo_no = (No_coluna_AVL*)malloc(sizeof(No_coluna_AVL));
    novo_no->coluna = coluna;
    novo_no->valor = valor;
    novo_no->direita = NULL;
    novo_no->esquerda = NULL;
    novo_no->altura = 1;
    return novo_no;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int obter_balanceamento_coluna(No_coluna_AVL *N) {
    if (N == NULL) return 0;
    return altura_coluna(N->esquerda) - altura_coluna(N->direita);
}

int obter_balanceamento_linha(No_linha_AVL *N) {
    if (N == NULL) return 0;
    return altura_linha(N->esquerda) - altura_linha(N->direita);
}

int altura_linha(No_linha_AVL *N) {
    if (N == NULL) return 0;
    return N->altura;
}

int altura_coluna(No_coluna_AVL *N){
    if (N == NULL) return 0;
    return N->altura;
};

No_coluna_AVL* rotacao_direita_coluna(No_coluna_AVL *y) {
    No_coluna_AVL *x = y->esquerda;
    No_coluna_AVL *T2 = x->direita;

    x->direita = y;
    y->esquerda = T2;

    y->altura = max(altura_coluna(y->esquerda), altura_coluna(y->direita)) + 1;
    x->altura = max(altura_coluna(x->esquerda), altura_coluna(x->direita)) + 1;

    return x;
}

No_coluna_AVL* rotacao_esquerda_coluna(No_coluna_AVL *x) {
    No_coluna_AVL *y = x->direita;
    No_coluna_AVL *T2 = y->esquerda;

    y->esquerda = x;
    x->direita = T2;

    x->altura = max(altura_coluna(x->esquerda), altura_coluna(x->direita)) + 1;
    y->altura = max(altura_coluna(y->esquerda), altura_coluna(y->direita)) + 1;

    return y;
}

No_linha_AVL* rotacao_direita_linha(No_linha_AVL *y) {
    No_linha_AVL *x = y->esquerda;
    No_linha_AVL *T2 = x->direita;

    x->direita = y;
    y->esquerda = T2;

    y->altura = max(altura_linha(y->esquerda), altura_linha(y->direita)) + 1;
    x->altura = max(altura_linha(x->esquerda), altura_linha(x->direita)) + 1;

    return x;
}

No_linha_AVL* rotacao_esquerda_linha(No_linha_AVL *x) {
    No_linha_AVL *y = x->direita;
    No_linha_AVL *T2 = y->esquerda;

    y->esquerda = x;
    x->direita = T2;

    x->altura = max(altura_linha(x->esquerda), altura_linha(x->direita)) + 1;
    y->altura = max(altura_linha(y->esquerda), altura_linha(y->direita)) + 1;

    return y;
}

// Funções de inserção e remoção:
No_linha_AVL* inserir_linha(No_linha_AVL* no, int linha) {


}

No_coluna_AVL* inserir_coluna(No_coluna_AVL* no, int coluna, int valor) {

}

No_linha_AVL* remover_linha(No_linha_AVL* no, int linha) {

}

No_coluna_AVL* remover_coluna(No_coluna_AVL* no, int coluna) {

}