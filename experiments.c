#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "hash_matrix.h"
#include "avl_matrix.h"

static void _allocation_fail(){
    fprintf(stderr, "Error: memory allocation failed.\n");
    exit(EXIT_FAILURE);
}

void generate_data(int n, int m, int k, int* I, int* J, float* Data){
    unsigned long long total_cells = (unsigned long long)n * (unsigned long long)m;
    char *has_been_selected = (char*) calloc(total_cells, sizeof(char));
    if(!has_been_selected){
        _allocation_fail();
    }
    for(int count = 0; count < k;){
        int i = rand() % n;
        int j = rand() % m;
        unsigned long long pos = (unsigned long long)i * (unsigned long long)m + (unsigned long long)j;
        if(!has_been_selected[pos]){
            has_been_selected[pos] = 1;
            I[count] = i;
            J[count] = j;
            Data[count] = ((float) rand()) / ((float) RAND_MAX);
            count++;
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
    return matrix;
}

void fill_dense_matrix(float** matrix, int k, int* I, int* J, float* Data){
    for(int count = 0; count < k; count++){
        matrix[I[count]][J[count]] = Data[count];
    }
    return;
}

AVLStatus fill_avl_matrix(AVLMatrix* matrix, int k, int* I, int* J, float* Data){
    for(int count = 0; count < k; count++){
        AVLStatus status;
        status = insert_element_avl(matrix, Data[count], I[count], J[count]);
        if(status != AVL_STATUS_OK){
            return status;
        }
    }
    return AVL_STATUS_OK;
}

HashStatus fill_hash_matrix(HashMatrix* matrix, int k, int* I, int* J, float* Data){
    for(int count = 0; count < k; count++){
        HashStatus status;
        status = set_element_hash(matrix, I[count], J[count], Data[count]);
        if(status != HASH_STATUS_OK){
            return status;
        }
    }
    return HASH_STATUS_OK;
}

static unsigned long long int _count_i_nodes_size(InnerNode* inner_tree){
    if(!inner_tree){
        return 0;
    }

    unsigned long long int left = _count_i_nodes_size(inner_tree-> left);
    unsigned long long int right = _count_i_nodes_size(inner_tree->right);
    return left + right + (unsigned long long int)sizeof(InnerNode);
}

static unsigned long long int _count_o_nodes_size(OuterNode* outer_tree){
    if(!outer_tree){
        return 0;
    }
    unsigned long long int left = _count_o_nodes_size(outer_tree->left);
    unsigned long long int right = _count_o_nodes_size(outer_tree->right);
    return left + right + (unsigned long long int)sizeof(OuterNode) + _count_i_nodes_size(outer_tree->inner_tree);
}

static unsigned long long int _avl_matrix_size(AVLMatrix* matrix){
    if(!matrix){
        return 0;
    }
    return (unsigned long long int)sizeof(AVLMatrix) + _count_o_nodes_size(matrix->main_root) + _count_o_nodes_size(matrix->transposed_root); 
}

static unsigned long long int _dense_matrix_size(int n, int m){
    return (unsigned long long int)n * (unsigned long long int)m * (unsigned long long int)sizeof(float);
}

static unsigned long long int _bucket_size(Node* bucket){
    if(!bucket){
        return 0;
    }

    return (unsigned long long int)sizeof(Node) + _bucket_size(bucket->next);
}

static unsigned long long int _hash_matrix_size(HashMatrix* matrix){
    if(!matrix){
        return 0;
    }
    unsigned long long int bucket_acc = 0;
    for(int i = 0; i < matrix->capacity; i++){
        bucket_acc = bucket_acc + (unsigned long long int) sizeof(Node*) + _bucket_size(matrix->buckets[i]);
    }
    return (unsigned long long int) sizeof(HashMatrix) + bucket_acc;
}

static double _delta_t_ns(struct timespec a, struct timespec b){
    return (b.tv_sec - a.tv_sec) * 1e9 + (b.tv_nsec - a.tv_nsec);
}

int main(){
    const int EXPERIMENT_MATRIX_LENGTH[] = {100, 100, 100, 100, 1000, 1000, 1000, 1000, 10000, 10000, 10000, 100000, 100000, 100000, 1000000, 1000000, 1000000};
    const float EXPERIMENT_SPARSITY[] = {0.01f, 0.05f, 0.1f, 0.2f, 0.01f, 0.05f, 0.1f, 0.2f, 1e-6f, 1e-7f, 1e-8f, 1e-7f, 1e-8f, 1e-9f, 1e-8f, 1e-9f, 1e-10f};
    const int NUM_EXPERIMENTS = 17;

    FILE *sizeExperimentsFile;

    sizeExperimentsFile = fopen("size_experiments.csv", "a");

    if(!sizeExperimentsFile){
        fprintf(stderr, "Error: couldn't open or create size_experiments.csv.\n");
        return 1;
    }

    for(int experiment = 0; experiment < NUM_EXPERIMENTS; experiment++){ //Experimentos de tamanho na memória
        int matrix_length = EXPERIMENT_MATRIX_LENGTH[experiment];
        float sparsity = EXPERIMENT_SPARSITY[experiment];
        unsigned long long side = (unsigned long long)matrix_length;
        unsigned long long cells = side * side;
        int k = (int) floor((double)cells * (double)sparsity);
        int* I = (int*) malloc(sizeof(int) * k);
        int* J = (int*) malloc(sizeof(int) * k);
        float* Data = (float*) malloc(sizeof(float) * k);
        if(!I || !J || !Data){
            if(I){
                free(I);
            }
            if(J){
                free(J);
            }
            if(Data){
                free(Data);
            }
            _allocation_fail();
        }
        generate_data(matrix_length, matrix_length, k, I, J, Data);
        unsigned long long int dense_matrix_size = _dense_matrix_size(matrix_length, matrix_length);
        AVLMatrix* avlmatrix;
        avlmatrix = create_matrix_avl(matrix_length, matrix_length);
        AVLStatus avlstatus = fill_avl_matrix(avlmatrix, k, I, J, Data);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error filling AVL matrix (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(avlmatrix);
            fclose(sizeExperimentsFile);
            return 1;
        }
        unsigned long long int avlmatrix_size = _avl_matrix_size(avlmatrix);
        free_matrix_avl(avlmatrix);
        HashMatrix* hashmatrix;
        hashmatrix = create_hash_matrix(matrix_length, matrix_length);
        HashStatus hashstatus = fill_hash_matrix(hashmatrix, k, I, J, Data);
        if(hashstatus != HASH_STATUS_OK){
            fprintf(stderr, "Error filling hash matrix (status %d).\n", hashstatus);
            free(I);
            free(J);
            free(Data);
            free_hash_matrix(hashmatrix);
            fclose(sizeExperimentsFile);
            return 1;
        }
        unsigned long long int hashmatrix_size = _hash_matrix_size(hashmatrix);
        free_hash_matrix(hashmatrix);

        fprintf(sizeExperimentsFile, "%d, %.12f, %d, %llu, %llu, %llu\n",
                matrix_length, sparsity, k,
                dense_matrix_size, avlmatrix_size, hashmatrix_size);

        free(I);
        free(J);
        free(Data);
    }
    fclose(sizeExperimentsFile);


    return 0;
}
