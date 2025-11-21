#include <stdio.h>
#include <stdlib.h>
#include "hash_matrix.h"
#include "AVL_Matrix.h"

static void _allocation_fail(){
    fprintf(stderr, "Error: memory allocation failed.\n");
    exit(EXIT_FAILURE);
}

void generate_data(int n, int m, int k, int* I, int* J, float* Data){
    char *has_been_selected = (char*) calloc(n*m, sizeof(char));
    if(!has_been_selected){
        _allocation_fail();
    }
    for(int count = 0; count < k;){
        int i = rand() % n;
        int j = rand() % m;
        if(!has_been_selected[i*m + j]){
            has_been_selected[i*m + j] = 1;
            I[count] = i;
            J[count] = j;
            Data[count] = ((float) rand()) / ((float) RAND_MAX);
        }
    }
    free(has_been_selected);
}

float** create_matrix(int n, int m){
    float** matrix = (float**) malloc(n * sizeof(float*));
    if(!matrix){
        _allocation_fail();
    }
    for(int i = 0; i < n; i++){
        matrix[i] = (float*) calloc(m, sizeof(float));
        if(!matrix[i]){
            for(int j = 0; j < i; j++){
                free(matrix[j]);
            }
            free(matrix);
            _allocation_fail();
        }
    }
}

void fill_matrices(float** regular_matrix, AVLMatrix* avl_matrix, HashMatrix hash_matrix, int k, int* I, int* J, float* Data){
    for(int count = 0; count < k; count++){
        AVLStatus status;
        regular_matrix[I[count]][J[count]] = Data[count];
        status = insert_element_avl(avl_matrix, Data[count], I[count], J[count]);
        if(status != AVL_STATUS_OK){
            //PRINTA STATUS, TODO
            return;
        }
        setElement(hash_matrix, I[count], J[count], Data[count]);
    }
}