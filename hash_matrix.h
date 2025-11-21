#pragma once
#include <stdbool.h>

/**
 * @file hash_Matrix.h
 * @brief Estruturas e operações para matrizes esparsas baseadas em hash.
 */

/**
 * @brief Nó que contém os elementos em cada posição da tabela de espalhamento.
 *
 * Armazena o índice da linha ou coluna, o valor do elemento não-nulo,
 * e o ponteiro para o próximo nó na lista encadeada do elemento com hash colidido.
 */
typedef struct node {
    int row, column;
    float data;
    struct node* next;
}node;

/**
 * @brief Representação de uma matriz esparsa usando tabela de espalhamento (hash table).
 * 
 * Armazena os buckets da tabela de espalhamento, capacidade total, número de elementos não nulos,
 * dimensões da matriz (linhas e colunas) e flag de transposição.
 */
typedef struct hash_matrix{
    node **buckets;
    int capacity, count, rows, columns; //capacity = tamanho total do hash, count = num de elementos não nulos
    bool is_transposed;
}hash_matrix;

/**
 * @brief Códigos de retorno das operações na matriz hash.
 */
typedef enum {
    HASH_STATUS_OK = 0,                 /**< Operação concluída com sucesso. */
    HASH_STATUS_NOT_FOUND = 1,          /**< Elemento solicitado não existe. */
    HASH_ERROR_NULL_MATRIX = -1,        /**< Ponteiro de matriz nulo. */
    HASH_ERROR_OUT_OF_BOUNDS = -2,      /**< Índices fora dos limites da matriz. */
    HASH_ERROR_DIMENSION_MISMATCH = -3, /**< Incompatibilidade de dimensões entre matrizes. */
    HASH_ERROR_INVALID_ARGUMENT = -4,   /**< Parâmetro inválido. */
    HASH_ERROR_NOT_IMPLEMENTED = -5     /**< Funcionalidade ainda não implementada. */
} hash_status;

/**
 * @brief Cria uma nova matriz hash com dimensões especificadas.
 *
 * @param rows número de linhas.
 * @param columns número de colunas.
 * @return Ponteiro para a nova matriz hash.
 */
hash_matrix* create_hash_matrix(int rows, int columns);

/**
 * @brief Obtém o valor de um elemento da matriz hash.
 * 
 * @param matrix ponteiro para a matriz hash.
 * @param row índice de linha.
 * @param column índice de coluna.
 * @return valor do elemento na posição (row, column) ou código de erro.
 */
float get_element(hash_matrix* matrix, int row, int column);

/**
 * @brief Define o valor de um elemento na matriz hash.
 * 
 * @param matrix ponteiro para a matriz hash.
 * @param row índice de linha.
 * @param column índice de coluna.
 * @param data valor a ser definido.
 * @return Código ::hash_status indicando sucesso ou motivo da falha.
 */
hash_status set_element(hash_matrix* matrix, int row, int column, float data);

/**
 * @brief Multiplica duas matrizes hash.
 * 
 * @param A ponteiro para a matriz hash A.
 * @param B ponteiro para a matriz hash B.
 * @return Ponteiro para a matriz resultante C.
 */
hash_matrix* matrix_multiplication(hash_matrix* A, hash_matrix* B);

/**
 * @brief Soma duas matrizes hash.
 * 
 * @param A ponteiro para a matriz hash A.
 * @param B ponteiro para a matriz hash B.
 * @return Ponteiro para a matriz resultante C.
 */
hash_matrix* matrix_addition(hash_matrix* A, hash_matrix* B);

/**
 * @brief Multiplica uma matriz hash por um escalar.
 * 
 * @param A ponteiro para a matriz hash A.
 * @param scalar valor escalar.
 * @return Ponteiro para a nova matriz resultante B.
 */
hash_matrix* matrix_scalar_multiplication(hash_matrix* A, float scalar);

/**
 * @brief Transpõe a matriz hash.
 * 
 * @param matrix ponteiro para a matriz hash.
 * @return Código ::hash_status indicando sucesso ou motivo da falha.
 */
hash_status transpose(hash_matrix* matrix);

/**
 * @brief Libera a memória alocada para a matriz hash.
 * 
 * @param matrix ponteiro para a matriz hash a ser liberada.
 * @return Código ::hash_status indicando sucesso ou motivo da falha.
 */
hash_status free_hash_matrix(hash_matrix* matrix);