#pragma once

/**
 * @file avl_matrix.h
 * @brief Estruturas e operações para matrizes esparsas baseadas em árvores AVL.
 */

/**
 * @brief Nó da árvore interna (contém os elementos).
 *
 * Armazena o índice da linha ou coluna, o valor do elemento não-nulo,
 * os ponteiros dos filhos da árvore AVL interna e a altura
 * relativa ao nó.
 */
typedef struct InnerNode{
    int key;
    float data;
    struct InnerNode* left;
    struct InnerNode* right;
    int height;
} InnerNode;

/**
 * @brief Nó da árvore externa (linha ou coluna da matriz).
 *
 * Cada nó representa uma linha ou coluna da matriz e aponta 
 * para a árvore interna contendo os elementos não-nulos da
 * linha ou coluna. Armazena o índice dessa linha ou coluna,
 * os ponteiros dos filhos da árvore AVL externa e a altura
 * relativa ao nó.
 */
typedef struct OuterNode{
    int key;
    struct InnerNode* inner_tree; 
    struct OuterNode* left;
    struct OuterNode* right;
    int height;
} OuterNode;

/**
 * @brief Códigos de retorno das operações na matriz AVL.
 */
typedef enum {
    AVL_STATUS_OK = 0,                 /**< Operação concluída com sucesso. */
    AVL_STATUS_NOT_FOUND = 1,          /**< Elemento solicitado não existe. */
    AVL_ERROR_NULL_MATRIX = -1,        /**< Ponteiro de matriz nulo. */
    AVL_ERROR_OUT_OF_BOUNDS = -2,      /**< Índices fora dos limites da matriz. */
    AVL_ERROR_DIMENSION_MISMATCH = -3, /**< Incompatibilidade de dimensões entre matrizes. */
    AVL_ERROR_INVALID_ARGUMENT = -4,   /**< Parâmetro inválido. */
    AVL_ERROR_NOT_IMPLEMENTED = -5     /**< Funcionalidade ainda não implementada. */
} AVLStatus;

/**
 * @brief Representação de uma matriz esparsa usando duas árvores AVL.
 *
 * A árvore main_root guarda os elementos no formato linha->coluna e a
 * transposed_root armazena a matriz transposta para facilitar operações.
 */
typedef struct AVLMatrix{
    struct OuterNode* main_root;       /**< Raiz (linhas) com árvores internas de colunas. */
    struct OuterNode* transposed_root; /**< Raiz (colunas) com árvores internas de linhas. */
    int k;                             /**< Quantidade de elementos não nulos. */
    int n;                             /**< Quantidade de linhas de main_root. */
    int m;                             /**< Quantidade de colunas de main_root. */
} AVLMatrix;

/**
 * @brief Obtém o valor de um elemento da matriz.
 *
 * @param matrix ponteiro para a matriz AVL.
 * @param i índice da linha.
 * @param j índice da coluna.
 * @param out_value ponteiro onde o valor será escrito (0.0 se ausente).
 * @return Código ::AVLStatus indicando sucesso ou motivo da falha.
 */
AVLStatus get_element_avl(AVLMatrix* matrix, int i, int j, float* out_value);

/**
 * @brief Insere ou atualiza um elemento na matriz.
 *
 * @param matrix ponteiro para a matriz AVL.
 * @param value valor a ser armazenado.
 * @param i índice da linha.
 * @param j índice da coluna.
 * @return Código ::AVLStatus indicando sucesso ou motivo da falha.
 */
AVLStatus insert_element_avl(AVLMatrix* matrix, float value, int i, int j);

/**
 * @brief Remove um elemento da matriz.
 *
 * @param matrix ponteiro para a matriz AVL.
 * @param i índice da linha.
 * @param j índice da coluna.
 * @return Código ::AVLStatus indicando sucesso ou motivo da falha.
 */
AVLStatus delete_element_avl(AVLMatrix* matrix, int i, int j);

/**
 * @brief Transpõe a matriz.
 *
 * @param matrix ponteiro para a matriz AVL.
 * @return Código ::AVLStatus indicando sucesso ou motivo da falha.
 */
AVLStatus transpose_avl(AVLMatrix* matrix);

/**
 * @brief Calcula B = a * A (possivelmente in-place).
 *
 * @param A matriz de entrada.
 * @param B matriz de saída (pode ser a mesma que A).
 * @param a fator escalar.
 * @return Código ::AVLStatus indicando sucesso ou motivo da falha.
 */
AVLStatus scalar_mul_avl(AVLMatrix* A, AVLMatrix* B, float a);

/**
 * @brief Calcula C = A + B.
 *
 * @param A primeira matriz de entrada.
 * @param B segunda matriz de entrada.
 * @param C matriz resultado.
 * @return Código ::AVLStatus indicando sucesso ou motivo da falha.
 */
AVLStatus sum_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C);

/**
 * @brief Calcula C = A * B (sem suporte a in-place).
 *
 * @param A matriz esquerda.
 * @param B matriz direita.
 * @param C matriz resultado pré-alocada com dimensões corretas.
 * @return Código ::AVLStatus indicando sucesso ou motivo da falha.
 */
AVLStatus matrix_mul_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C);

/**
 * @brief Converte um código ::AVLStatus em mensagem textual.
 *
 * @param status código de retorno.
 * @return String com a mensagem.
 */
const char* avl_status_string(AVLStatus status);

/**
 * @brief Cria uma matriz vazia de dimensões n x m.
 *
 * @param n número de linhas (não negativo).
 * @param m número de colunas (não negativo).
 * @return Ponteiro para nova matriz ou NULL em caso de parâmetros inválidos.
 */
AVLMatrix* create_matrix_avl(int n, int m);

/**
 * @brief Libera a memória associada à uma matriz AVL.
 *
 * @param matrix ponteiro para a matriz a ser destruída (ignorado se NULL).
 */
void free_matrix_avl(AVLMatrix* matrix);
