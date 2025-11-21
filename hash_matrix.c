#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hash_matrix.h"

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR_UPPER 0.75
#define LOAD_FACTOR_LOWER 0.25

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

hash_matrix* create_hash_matrix(int rows, int columns){
    if (rows < 0 || columns < 0){
        return HASH_ERROR_INVALID_ARGUMENT;
    }

    hash_matrix* matrix = malloc(sizeof(struct hash_matrix));
    assert(matrix != NULL);

    matrix->rows = rows;
    matrix->columns = columns;
    matrix->capacity = INITIAL_CAPACITY;
    matrix->count = 0;
    matrix->is_transposed = false;

    matrix->buckets = calloc(INITIAL_CAPACITY, sizeof(node));
    assert(matrix->buckets != NULL);

    return matrix;
}

/**
 * @brief Redimensiona a tabela de espalhamento da matriz hash.
 *
 * @param matrix ponteiro para a matriz hash a ser redimensionada.
 * @return Código ::hash_status indicando sucesso ou motivo da falha.
 */
hash_status resize(hash_matrix* matrix){
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

    node **new_buckets = calloc(new_capacity, sizeof(node));
    assert(new_buckets != NULL);

    for(int i = 0; i < matrix->capacity; i++){
        node* curr = matrix->buckets[i];
        while (curr != NULL){
            node* next = curr->next;
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

float get_element(hash_matrix* matrix, int row, int column){
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

    node* curr = matrix->buckets[index];
    while (curr != NULL){
        if (curr->row == target_row && curr->column == target_column){
            return curr->data;
        }
        curr = curr->next;
    }

    return 0.0;
}

hash_status set_element(hash_matrix* matrix, int row, int column, float data){
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
    node* curr = matrix->buckets[index];
    node* prev = NULL;

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
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    if (data != 0.0){
        if ((float)(matrix->count + 1) / matrix->capacity > LOAD_FACTOR_UPPER){
            resize(matrix);
            index = hash(target_row, target_column, matrix->capacity);
        }
        
        node* new_node = malloc(sizeof(node));
        assert(new_node != NULL);
        new_node->column = target_column;
        new_node->row = target_row;
        new_node->data = data;
        new_node->next = NULL;

        new_node->next = matrix->buckets[index];
        matrix->buckets[index] = new_node;
        matrix->count++;
    }
}

hash_matrix* matrix_multiplication(hash_matrix* A, hash_matrix* B){
    if (A == NULL || B == NULL){
        return HASH_ERROR_NULL_MATRIX;
    }

    if (A->is_transposed && B->is_transposed){
        if (A->columns != B->rows){
            return HASH_ERROR_DIMENSION_MISMATCH;
        }
    } else if (A->is_transposed && !B->is_transposed){
        if (A->columns != B->columns){
            return HASH_ERROR_DIMENSION_MISMATCH;
        }
    } else if (!A->is_transposed && B->is_transposed){
        if (A->rows != B->rows){
            return HASH_ERROR_DIMENSION_MISMATCH;
        }
    } else {
        if (A->rows != B->columns){
            return HASH_ERROR_DIMENSION_MISMATCH;
        }
    }

    hash_matrix* C = createhash_matrix(A->rows, B->columns);
    
    node* current_bucket_A = NULL;
    node* current_bucket_B = NULL;

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
                        float temp = getElement(C, row_a, column_b);
                        temp += data_a * data_b;
                        setElement(C, row_a, column_b, temp);
                    }
                    current_bucket_B = current_bucket_B->next;
                }
            }
            current_bucket_A = current_bucket_A->next;
        }
    }

    return C;
}

hash_matrix* matrix_addition(hash_matrix* A, hash_matrix* B){
    if (A == NULL || B == NULL){
        fprintf(stderr, "MATRIZ NULL");
        exit(1);
    }

    hash_matrix* C = createhash_matrix(A->rows, A->columns);

    node* current_bucket = NULL;

    for (int i = 0; i < A->capacity; i++){
        current_bucket = A->buckets[i];

        while (current_bucket != NULL){
            int row_a = A->is_transposed ? current_bucket->column : current_bucket->row;
            int column_a = A->is_transposed ? current_bucket->row : current_bucket->column;
            float data_a = current_bucket->data;

            float temp = getElement(C, row_a, column_a);
            temp += data_a;
            setElement(C, row_a, column_a, temp);

            current_bucket = current_bucket->next;
        }
    }

    for (int i = 0; i < B->capacity; i++){
        current_bucket = B->buckets[i];

        while (current_bucket != NULL){
            int row_b = B->is_transposed ? current_bucket->column : current_bucket->row;
            int column_b = B->is_transposed ? current_bucket->row : current_bucket->column;
            float data_b = current_bucket->data;

            float temp = getElement(C, row_b, column_b);
            temp += data_b;
            setElement(C, row_b, column_b, temp);

            current_bucket = current_bucket->next;
        }
    }

    return C;
}

hash_matrix* matrix_scalar_multiplication(hash_matrix* A, float scalar){
    if (A == NULL){
        return HASH_ERROR_NULL_MATRIX;
    }

    hash_matrix* B = createhash_matrix(A->rows, A->columns);

    node* current_bucket = NULL;

    for (int i = 0; i < A->capacity; i++){
        current_bucket = A->buckets[i];

        while (current_bucket != NULL){
            int row_a = A->is_transposed ? current_bucket->column : current_bucket->row;
            int column_a = A->is_transposed ? current_bucket->row : current_bucket->column;
            float data_a = current_bucket->data;

            float temp = data_a * scalar;
            setElement(B, row_a, column_a, temp);

            current_bucket = current_bucket->next;
        }
    }

    return B;
}

hash_status transpose(hash_matrix* matrix){
    if (matrix == NULL){
    return HASH_ERROR_NULL_MATRIX;
    }
    matrix->is_transposed = !matrix->is_transposed;
    return HASH_STATUS_OK;
}

hash_status free_hash_matrix(hash_matrix* matrix){
    if (matrix == NULL){
        return HASH_ERROR_NULL_MATRIX;
    }
    for (int i = 0; i < matrix->capacity; i++){
        node* curr = matrix->buckets[i];
        while (curr != NULL){
            node* next = curr->next;
            free(curr);
            curr = next;
        }
    }
    free(matrix->buckets);
    free(matrix);
    return HASH_STATUS_OK;
}