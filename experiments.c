#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hash_matrix.h"
#include "avl_matrix.h"

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

float** create_dense_matrix(int n, int m){
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

static unsigned int _count_i_nodes_size(InnerNode* inner_tree){
    if(!inner_tree){
        return 0;
    }

    unsigned int left = _count_i_nodes_size(inner_tree-> left);
    unsigned int right = _count_i_nodes_size(inner_tree->right);
    return left+right+((unsigned int)sizeof(InnerNode));
}

static unsigned int _count_o_nodes_size(OuterNode* outer_tree){
    if(!outer_tree){
        return 0;
    }
    unsigned int left = _count_o_nodes_size(outer_tree->left);
    unsigned int right = _count_o_nodes_size(outer_tree->right);
    return left + right + ((unsigned int)sizeof(OuterNode)) + _count_i_nodes_size(outer_tree->inner_tree);
}

static unsigned int _avl_matrix_size(AVLMatrix* matrix){
    if(!matrix){
        return 0;
    }
    return ((unsigned int)sizeof(AVLMatrix)) + _count_o_nodes_size(matrix->main_root) + _count_o_nodes_size(matrix->transposed_root); 
}

static unsigned int _dense_matrix_size(int n, int m){
    return ((unsigned int) n) * ((unsigned int) m) * ((unsigned int) sizeof(float));
}

static unsigned int _bucket_size(Node* bucket){
    if(!bucket){
        return 0;
    }

    return (unsigned int)sizeof(Node) + _bucket_size(bucket->next);
}

static unsigned int _hash_matrix_size(HashMatrix* matrix){
    if(!matrix){
        return 0;
    }
    unsigned int bucket_acc;
    for(int i = 0; i < matrix->capacity; i++){
        bucket_acc = bucket_acc + (unsigned int) sizeof(Node*) + _bucket_size(matrix->buckets[i]);
    }
    return (unsigned int) sizeof(HashMatrix) + bucket_acc;
}

static double _delta_t_ns(struct timespec a, struct timespec b){
    return (b.tv_sec - a.tv_sec) * 1e9 + (b.tv_nsec - a.tv_nsec);
}