#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hash_matrix.h"

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR_UPPER 0.75
#define LOAD_FACTOR_LOWER 0.25

/**
 * @brief Encerramento imediato em caso de falha de alocação.
 *
 * Imprime uma mensagem de erro em stderr e aborta o processo usando EXIT_FAILURE.
 */
static void _allocation_fail(){
    fprintf(stderr, "Error: memory allocation failed.\n");
    exit(EXIT_FAILURE);
}

/**
 * @brief retorna um hash dados inteiros de linha, coluna, e capacidade.
 *
 * @param row índice de linha.
 * @param column índice de coluna.
 * @param capacity capacidade total da tabela de espalhamento.
 * @return o hash calculado.
 */
unsigned int hash(int row, int column, int capacity){
    unsigned long h = ((unsigned long) row * 31337 + (unsigned long) column * 2731)%capacity;
    return (unsigned int) h;
}

/**
 * @brief Redimensiona a tabela de espalhamento da matriz hash.
 *
 * @param matrix ponteiro para a matriz hash a ser redimensionada.
 * @return Código ::HashStatus indicando sucesso ou motivo da falha.
 */
HashStatus resize(HashMatrix* matrix){
    if (matrix == NULL){
        return HASH_ERROR_NULL_MATRIX;
    }

    int new_capacity = matrix->capacity;
    
    if ((float)(matrix->count+1) / matrix->capacity > LOAD_FACTOR_UPPER){
        new_capacity = matrix->capacity *2;
    } else if ((float)(matrix->count) / matrix->capacity < LOAD_FACTOR_LOWER){
        new_capacity = matrix->capacity/2;
    }

    if (new_capacity == matrix->capacity){
        return HASH_STATUS_OK;
    }

    Node **new_buckets = calloc(new_capacity, sizeof(Node*));
    if (new_buckets == NULL){
        _allocation_fail();
    }

    for(int i = 0; i < matrix->capacity; i++){
        Node* curr = matrix->buckets[i];
        while (curr != NULL){
            Node* next = curr->next;
            unsigned long new_index = hash(curr->row, curr->column, new_capacity);

            curr->next = new_buckets[new_index];
            new_buckets[new_index] = curr;
            curr = next;
        }
    }

    free(matrix->buckets);
    matrix->buckets = new_buckets;
    matrix->capacity = new_capacity;

    return HASH_STATUS_OK;
}

/**
 * @brief Verifica se a matriz resultado está corretamente inicializada.
 * 
 * A matriz deve ser vazia.
 *
 * @param result ponteiro para a matriz resultado.
 * @param expected_rows número esperado de linhas.
 * @param expected_columns número esperado de colunas.
 * @return true se a matriz estiver correta, false caso contrário.
 */
bool verify_result_matrix(HashMatrix* result, int expected_rows, int expected_columns){
    if (result == NULL){
        return false;
    }

    int rows = result->is_transposed ? result->columns : result->rows;
    int columns = result->is_transposed ? result->rows : result->columns;
    if (rows != expected_rows || columns != expected_columns){
        return false;
    }

    if (result->count != 0){
        return false;
    }

    return true;
}

HashMatrix* create_hash_matrix(int rows, int columns){
    if (rows < 0 || columns < 0){
        return NULL;
    }

    HashMatrix* matrix = malloc(sizeof(struct HashMatrix));
    if (matrix == NULL){
        _allocation_fail();
    } 

    matrix->rows = rows;
    matrix->columns = columns;
    matrix->capacity = INITIAL_CAPACITY;
    matrix->count = 0;
    matrix->is_transposed = false;

    matrix->buckets = calloc(INITIAL_CAPACITY, sizeof(Node*));
    if (matrix->buckets == NULL){
        free(matrix);
        _allocation_fail();
    }

    return matrix;
}

float get_element_hash(HashMatrix* matrix, int row, int column){
    if (matrix == NULL){
        return HASH_ERROR_NULL_MATRIX;
    }

    int max_rows = matrix->is_transposed ? matrix->columns : matrix->rows;
    int max_columns = matrix->is_transposed ? matrix->rows : matrix->columns;

    if (row >= max_rows || row < 0 || column >= max_columns || column < 0){
        return HASH_ERROR_OUT_OF_BOUNDS;
    }

    int target_row = matrix->is_transposed ? column : row;
    int target_column = matrix->is_transposed ? row : column;

    unsigned int index = hash(target_row, target_column, matrix->capacity);

    Node* curr = matrix->buckets[index];
    while (curr != NULL){
        if (curr->row == target_row && curr->column == target_column){
            return curr->data;
        }
        curr = curr->next;
    }

    return 0.0;
}

HashStatus set_element_hash(HashMatrix* matrix, int row, int column, float data){
    if (matrix == NULL){
        return HASH_ERROR_NULL_MATRIX;
    }

    int max_rows = matrix->is_transposed ? matrix->columns : matrix->rows;
    int max_columns = matrix->is_transposed ? matrix->rows : matrix->columns;

    if (row >= max_rows || row < 0 || column >= max_columns || column < 0){
        return HASH_ERROR_OUT_OF_BOUNDS;
    }

    int target_row = matrix->is_transposed ? column : row;
    int target_column = matrix->is_transposed ? row : column;

    unsigned int index = hash(target_row, target_column, matrix->capacity);
    Node* curr = matrix->buckets[index];
    Node* prev = NULL;

    while(curr != NULL){
        if (curr->row == target_row && curr->column == target_column){
            if (data == 0.0){
                if (prev == NULL){
                    matrix->buckets[index] = curr->next;
                } else {
                    prev->next = curr->next;
                }
                free(curr);
                matrix->count--;
                if ((float)matrix->count / matrix->capacity < LOAD_FACTOR_LOWER && matrix->capacity > INITIAL_CAPACITY) {
                    resize(matrix);
                }
            } else {
                curr->data = data;
            }
            return HASH_STATUS_OK;
        }
        prev = curr;
        curr = curr->next;
    }

    if (data != 0.0){
        if ((float)(matrix->count + 1) / matrix->capacity > LOAD_FACTOR_UPPER){
            resize(matrix);
            index = hash(target_row, target_column, matrix->capacity);
        }
        
        Node* new_Node = malloc(sizeof(Node));
        if (new_Node == NULL){
            _allocation_fail();
        }
        new_Node->column = target_column;
        new_Node->row = target_row;
        new_Node->data = data;
        new_Node->next = NULL;

        new_Node->next = matrix->buckets[index];
        matrix->buckets[index] = new_Node;
        matrix->count++;
    }

    return HASH_STATUS_OK;
}

HashStatus matrix_multiplication_hash(HashMatrix* A, HashMatrix* B, HashMatrix* C){
    if (A == NULL || B == NULL){
        return HASH_ERROR_NULL_MATRIX;
    }

    int columns_a = A->is_transposed ? A->rows : A->columns;
    int columns_b = B->is_transposed ? B->rows : B->columns;
    int rows_a = A->is_transposed ? A->columns : A->rows;
    int rows_b = B->is_transposed ? B->columns : B->rows;
    if (columns_a != rows_b){
        return HASH_ERROR_DIMENSION_MISMATCH;
    }

    if (!verify_result_matrix(C, rows_a, columns_b)){
        return HASH_ERROR_INVALID_ARGUMENT;
    }    

    Node* current_bucket_A = NULL;
    Node* current_bucket_B = NULL;

    for (int i = 0; i < A->capacity; i++){
        current_bucket_A = A->buckets[i];

        while (current_bucket_A != NULL){
            int row_a = A->is_transposed ? current_bucket_A->column : current_bucket_A->row;
            int column_a = A->is_transposed ? current_bucket_A->row : current_bucket_A->column;
            float data_a = current_bucket_A->data;

            for (int j = 0; j < B->capacity; j++){
                current_bucket_B = B->buckets[j];
                while (current_bucket_B != NULL){
                    int row_b = B->is_transposed ? current_bucket_B->column : current_bucket_B->row;
                    int column_b = B->is_transposed ? current_bucket_B->row : current_bucket_B->column;
                    float data_b = current_bucket_B->data;

                    if (column_a == row_b){
                        float temp = get_element_hash(C, row_a, column_b);
                        temp += data_a * data_b;
                        set_element_hash(C, row_a, column_b, temp);
                    }
                    current_bucket_B = current_bucket_B->next;
                }
            }
            current_bucket_A = current_bucket_A->next;
        }
    }

    return HASH_STATUS_OK;
}

HashStatus matrix_addition_hash(HashMatrix* A, HashMatrix* B, HashMatrix* C){
    if (A == NULL || B == NULL){
        fprintf(stderr, "MATRIZ NULL");
        exit(1);
    }

    if (!verify_result_matrix(C, A->is_transposed ? A->columns : A->rows, A->is_transposed ? A->rows : A->columns)){
        return HASH_ERROR_INVALID_ARGUMENT;
    }
    
    Node* current_bucket = NULL;

    for (int i = 0; i < A->capacity; i++){
        current_bucket = A->buckets[i];

        while (current_bucket != NULL){
            int row_a = A->is_transposed ? current_bucket->column : current_bucket->row;
            int column_a = A->is_transposed ? current_bucket->row : current_bucket->column;
            float data_a = current_bucket->data;

            float temp = get_element_hash(C, row_a, column_a);
            temp += data_a;
            set_element_hash(C, row_a, column_a, temp);

            current_bucket = current_bucket->next;
        }
    }

    for (int i = 0; i < B->capacity; i++){
        current_bucket = B->buckets[i];

        while (current_bucket != NULL){
            int row_b = B->is_transposed ? current_bucket->column : current_bucket->row;
            int column_b = B->is_transposed ? current_bucket->row : current_bucket->column;
            float data_b = current_bucket->data;

            float temp = get_element_hash(C, row_b, column_b);
            temp += data_b;
            set_element_hash(C, row_b, column_b, temp);

            current_bucket = current_bucket->next;
        }
    }

    return HASH_STATUS_OK;
}

HashStatus matrix_scalar_multiplication_hash(HashMatrix* A, HashMatrix* B, float scalar){
    if (A == NULL){
        return HASH_ERROR_NULL_MATRIX;
    }

    if (!verify_result_matrix(B, A->is_transposed ? A->columns : A->rows, A->is_transposed ? A->rows : A->columns)){
        return HASH_ERROR_INVALID_ARGUMENT;
    }

    Node* current_bucket = NULL;

    for (int i = 0; i < A->capacity; i++){
        current_bucket = A->buckets[i];

        while (current_bucket != NULL){
            int row_a = A->is_transposed ? current_bucket->column : current_bucket->row;
            int column_a = A->is_transposed ? current_bucket->row : current_bucket->column;
            float data_a = current_bucket->data;

            float temp = data_a * scalar;
            set_element_hash(B, row_a, column_a, temp);

            current_bucket = current_bucket->next;
        }
    }

    return HASH_STATUS_OK;
}

HashStatus transpose_hash(HashMatrix* matrix){
    if (matrix == NULL){
    return HASH_ERROR_NULL_MATRIX;
    }
    matrix->is_transposed = !matrix->is_transposed;
    return HASH_STATUS_OK;
}

HashStatus free_hash_matrix(HashMatrix* matrix){
    if (matrix == NULL){
        return HASH_ERROR_NULL_MATRIX;
    }
    for (int i = 0; i < matrix->capacity; i++){
        Node* curr = matrix->buckets[i];
        while (curr != NULL){
            Node* next = curr->next;
            free(curr);
            curr = next;
        }
    }
    free(matrix->buckets);
    free(matrix);
    return HASH_STATUS_OK;
}