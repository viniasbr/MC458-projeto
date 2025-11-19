#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hash_matrix.h"

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR_UPPER 0.75
#define LOAD_FACTOR_LOWER 0.25

unsigned int hash(int row, int column, int capacity){
    unsigned long h = ((unsigned long) ((row * 31337)%capacity) + (unsigned long) ((column * 2731)%capacity))%capacity;
    return (unsigned int) h;
}

HashMatrix createHashMatrix(int rows, int columns){
    HashMatrix matrix = malloc(sizeof(struct _hashMatrix));
    assert(matrix != NULL);

    matrix->rows = rows;
    matrix->columns = columns;
    matrix->capacity = INITIAL_CAPACITY;
    matrix->count = 0;
    matrix->is_tranposed = false;

    matrix->buckets = calloc(INITIAL_CAPACITY, sizeof(Node));
    assert(matrix->buckets != NULL);

    return matrix;
}

void resize(HashMatrix matrix, float load_factor){
    int old_capacity = matrix->capacity;
    int new_capacity = load_factor == LOAD_FACTOR_UPPER ? matrix->capacity*2 : matrix->capacity/2;

    Node *new_buckets = calloc(new_capacity, sizeof(Node));
    assert(new_buckets != NULL);

    for(int i = 0; i < old_capacity; i++){
        Node curr = matrix->buckets[i];
        while (curr != NULL){
            Node next = curr->next;
            unsigned long new_index = hash(curr->row, curr->column, new_capacity);

            curr->next = new_buckets[new_index];
            new_buckets[new_index] = curr;
            curr = next;
        }
    }

    free(matrix->buckets);
    matrix->buckets = new_buckets;
    matrix->capacity = new_capacity;
}

//A princípio é pra ser O(1)
float getElement(HashMatrix matrix, int row, int column){
    int max_rows = matrix->is_tranposed ? matrix->columns : matrix->rows;
    int max_columns = matrix->is_tranposed ? matrix->rows : matrix->columns;

    //testa se está out of bounds
    if (row > max_rows || row < 0 || column > max_columns || column < 0){
        fprintf(stderr, "ERRO: BUSCA OUT OF BOUNDS");
        exit(1);
    }

    int target_row = matrix->is_tranposed ? column : row;
    int target_column = matrix->is_tranposed ? row : column;

    unsigned int index = hash(row, column, matrix->capacity);

    Node curr = matrix->buckets[index];
    while (curr != NULL){
        if (curr->row == target_row && curr->column == target_column){
            return curr->data;
        }
        curr = curr->next;
    }

    return 0.0;
}

void setElement(HashMatrix matrix, int row, int column, float data){

    int max_rows = matrix->is_tranposed ? matrix->columns : matrix->rows;
    int max_columns = matrix->is_tranposed ? matrix->rows : matrix->columns;

    //testa se está out of bounds
    if (row > max_rows || row < 0 || column > max_columns || column < 0){
        fprintf(stderr, "ERRO: INSERÇÃO OUT OF BOUNDS");
        exit(1);
    }

    int target_row = matrix->is_tranposed ? column : row;
    int target_column = matrix->is_tranposed ? row : column;

    unsigned int index = hash(row, column, matrix->capacity);
    Node curr = matrix->buckets[index], prev = NULL;

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
            } else {
                curr->data = data;
            }
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    if (data != 0.0){
        if ((float)(matrix->count+1) / matrix->capacity > LOAD_FACTOR_UPPER){
            resize(matrix, LOAD_FACTOR_UPPER);
            index = hash(row, column, matrix->capacity);
        } else if ((float)(matrix->count+1) / matrix->capacity < LOAD_FACTOR_LOWER){
            resize(matrix, LOAD_FACTOR_LOWER);
            index = hash(row, column, matrix->capacity);
        }
        Node new_node = malloc(sizeof(struct _node));
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

void transpose(HashMatrix matrix){
    //as dimensões não são FISICAMENTE trocadas, mas uma checagem de is_transposed diz qual sua relação
    matrix->is_tranposed = !matrix->is_tranposed;
}