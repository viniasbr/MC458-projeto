#define _POSIX_C_SOURCE 200809L
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
    AVLMatrix* visited = create_matrix_avl(n, m);
    if(!visited){
        _allocation_fail();
    }

    for(int count = 0; count < k;){
        int i = rand() % n;
        int j = rand() % m;

        float already = 0.0f;
        AVLStatus status = get_element_avl(visited, i, j, &already);
        if(status != AVL_STATUS_OK){
            free_matrix_avl(visited);
            _allocation_fail();
        }
        if(already == 0.0f){
            status = insert_element_avl(visited, 1.0f, i, j);
            if(status != AVL_STATUS_OK){
                free_matrix_avl(visited);
                _allocation_fail();
            }
            I[count] = i;
            J[count] = j;
            Data[count] = ((float) rand()) / ((float) RAND_MAX);
            count++;
        }
    }
    free_matrix_avl(visited);
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

/* Operações básicas para matriz densa (float**) */
static float dense_get(float** matrix, int i, int j){
    return matrix[i][j];
}

static void dense_set(float** matrix, int i, int j, float value){
    matrix[i][j] = value;
}

static void dense_transpose(float** A, float** AT, int n, int m){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            AT[j][i] = A[i][j];
        }
    }
}

static void dense_scalar_mul(float** A, float** B, int n, int m, float a){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            B[i][j] = A[i][j] * a;
        }
    }
}

static void dense_sum(float** A, float** B, float** C, int n, int m){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            C[i][j] = A[i][j] + B[i][j];
        }
    }
}

static void dense_mul(float** A, float** B, float** C, int n, int k, int m){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            float acc = 0.0f;
            for(int p = 0; p < k; p++){
                acc += A[i][p] * B[p][j];
            }
            C[i][j] = acc;
        }
    }
}

int main(){
    srand(42);
    const int EXPERIMENT_MATRIX_LENGTH[] = {100, 100, 100, 100, 1000, 1000, 1000, 1000, 10000, 10000, 10000, 100000, 100000, 100000, 1000000, 1000000, 1000000};
    const float EXPERIMENT_SPARSITY[] = {0.01f, 0.05f, 0.1f, 0.2f, 0.01f, 0.05f, 0.1f, 0.2f, 1e-8f, 1e-7f, 1e-6f, 1e-9f, 1e-8f, 1e-7f, 1e-10f, 1e-9f, 1e-8f};
    const int NUM_EXPERIMENTS = 17;
    const int NUM_DENSE_EXPERIMENTS = 8;

    FILE *sizeExperimentsFile;

    sizeExperimentsFile = fopen("size_experiments.csv", "w");

    if(!sizeExperimentsFile){
        fprintf(stderr, "Error: couldn't open or create size_experiments.csv.\n");
        return 1;
    }
    fprintf(sizeExperimentsFile, "n,sparsity,k,dense_bytes,avl_bytes,hash_bytes\n");

    for(int experiment = 0; experiment < NUM_EXPERIMENTS; experiment++){ //Experimentos de tamanho na memória
        int matrix_length = EXPERIMENT_MATRIX_LENGTH[experiment];
        float sparsity = EXPERIMENT_SPARSITY[experiment];
        unsigned long long side = (unsigned long long)matrix_length;
        unsigned long long cells = side * side;
        int k = (int) ceil((double)cells * (double)sparsity);
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

    FILE* timeExperimentsFile = fopen("time_experiments.csv", "w");

    if(!timeExperimentsFile){
        fprintf(stderr, "Error: couldn't open or create time_experiments.csv.\n");
        return 1;
    }
    fprintf(timeExperimentsFile, "n,sparsity,k,dense_get_ns,dense_set_ns,dense_trans_ns,dense_scalar_ns,dense_sum_ns,dense_mul_ns,avl_get_ns,avl_set_ns,avl_trans_ns,avl_scalar_ns,avl_sum_ns,avl_mul_ns,hash_get_ns,hash_set_ns,hash_trans_ns,hash_scalar_ns,hash_sum_ns,hash_mul_ns\n");
    
    for(int experiment = 0; experiment < NUM_DENSE_EXPERIMENTS; experiment++){ //Experimentos de tempo até o limite do denso
        int matrix_length = EXPERIMENT_MATRIX_LENGTH[experiment];
        float sparsity = EXPERIMENT_SPARSITY[experiment];
        struct timespec t0, t1;
        unsigned long long side = (unsigned long long)matrix_length;
        unsigned long long cells = side * side;
        int k = (int) ceil((double)cells * (double)sparsity);
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
        /* DENSE */
        float** dense_A = create_dense_matrix(matrix_length, matrix_length);
        float** dense_B = create_dense_matrix(matrix_length, matrix_length);
        float** dense_AT = create_dense_matrix(matrix_length, matrix_length);
        float** dense_scalar_out = create_dense_matrix(matrix_length, matrix_length);
        float** dense_sum_out = create_dense_matrix(matrix_length, matrix_length);
        float** dense_mul_out = create_dense_matrix(matrix_length, matrix_length);
        if(!dense_A || !dense_B || !dense_AT || !dense_scalar_out || !dense_sum_out || !dense_mul_out){
            _allocation_fail();
        }
        fill_dense_matrix(dense_A, k, I, J, Data);
        fill_dense_matrix(dense_B, k, I, J, Data);
        int pos_dense = rand() % k;
        int i_dense = I[pos_dense];
        int j_dense = J[pos_dense];
        printf("Dense get (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        float dense_val = dense_get(dense_A, i_dense, j_dense);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dense_get_t = _delta_t_ns(t0, t1);
        printf("Dense set (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        dense_set(dense_A, i_dense, j_dense, 3.14f);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dense_set_t = _delta_t_ns(t0, t1);
        printf("Dense transpose (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        dense_transpose(dense_A, dense_AT, matrix_length, matrix_length);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dense_trans_t = _delta_t_ns(t0, t1);
        printf("Dense scalar mul (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        dense_scalar_mul(dense_A, dense_scalar_out, matrix_length, matrix_length, 3.0f);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dense_scalar = _delta_t_ns(t0, t1);
        printf("Dense sum (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        dense_sum(dense_A, dense_B, dense_sum_out, matrix_length, matrix_length);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dense_sum_t = _delta_t_ns(t0, t1);
        printf("Dense mul (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        dense_mul(dense_A, dense_B, dense_mul_out, matrix_length, matrix_length, matrix_length);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dense_mul_t = _delta_t_ns(t0, t1);
        for(int x = 0; x < matrix_length; x++){
            free(dense_A[x]);
            free(dense_B[x]);
            free(dense_AT[x]);
            free(dense_scalar_out[x]);
            free(dense_sum_out[x]);
            free(dense_mul_out[x]);
        }
        free(dense_A);
        free(dense_B);
        free(dense_AT);
        free(dense_scalar_out);
        free(dense_sum_out);
        free(dense_mul_out);
        /* AVL */
        AVLMatrix* A = create_matrix_avl(matrix_length, matrix_length);
        AVLMatrix* B = create_matrix_avl(matrix_length, matrix_length);
        AVLMatrix* scalar_out = create_matrix_avl(matrix_length, matrix_length);
        AVLMatrix* sum_out = create_matrix_avl(matrix_length, matrix_length);
        AVLMatrix* mul_out = create_matrix_avl(matrix_length, matrix_length);
        if(!A || !B || !scalar_out || !sum_out || !mul_out){
            free(I);
            free(J);
            free(Data);
            if(A){free_matrix_avl(A);}
            if(B){free_matrix_avl(B);}
            if(scalar_out){free_matrix_avl(scalar_out);}
            if(sum_out){free_matrix_avl(sum_out);}
            if(mul_out){free_matrix_avl(mul_out);}
            fclose(timeExperimentsFile);
            return 1;
        }
        AVLStatus avlstatus = fill_avl_matrix(A, k, I, J, Data);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error filling AVL matrix (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        avlstatus = fill_avl_matrix(B, k, I, J, Data);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error filling AVL matrix B (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        int pos = rand() % k;
        int i = I[pos];
        int j = J[pos];
        float value;
        printf("AVL get (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = get_element_avl(A, i, j, &value);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error getting AVL at position (%d, %d) (status %d: %s).\n",
                    i,j,avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_get = _delta_t_ns(t0, t1);
        printf("AVL set (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = insert_element_avl(A, 3.14f ,i, j);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error inserting element on AVL at position (%d, %d) and value 3.14f (status %d: %s).\n",
                    i,j,avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_set = _delta_t_ns(t0, t1);
        printf("AVL transpose (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = transpose_avl(A);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error transposing AVL matrix (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_trans = _delta_t_ns(t0, t1);
        avlstatus = transpose_avl(A);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error transposing AVL matrix back (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }

        printf("AVL scalar mul (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = scalar_mul_avl(A, scalar_out, 3.0f);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error on AVL scalar multiplication (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_scalar = _delta_t_ns(t0, t1);

        printf("AVL sum (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = sum_avl(A, B, sum_out);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error on AVL sum (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_sum_t = _delta_t_ns(t0, t1);

        printf("AVL mul (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = matrix_mul_avl(A, B, mul_out);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error on AVL matrix multiplication (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_mul = _delta_t_ns(t0, t1);

        /* HASH */
        printf("Hash setup (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        HashMatrix* H1 = create_hash_matrix(matrix_length, matrix_length);
        HashMatrix* H2 = create_hash_matrix(matrix_length, matrix_length);
        HashMatrix* H_scalar = create_hash_matrix(matrix_length, matrix_length);
        HashMatrix* H_sum = create_hash_matrix(matrix_length, matrix_length);
        HashMatrix* H_mul = create_hash_matrix(matrix_length, matrix_length);
        if(!H1 || !H2 || !H_scalar || !H_sum || !H_mul){
            _allocation_fail();
        }
        HashStatus hstatus = fill_hash_matrix(H1, k, I, J, Data);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        hstatus = fill_hash_matrix(H2, k, I, J, Data);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        int pos_h = rand() % k;
        int ih = I[pos_h];
        int jh = J[pos_h];
        printf("Hash get (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        float hval = get_element_hash(H1, ih, jh);
        (void)hval;
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double hash_get_t = _delta_t_ns(t0, t1);
        printf("Hash set (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        hstatus = set_element_hash(H1, ih, jh, 3.14f);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        double hash_set_t = _delta_t_ns(t0, t1);
        printf("Hash transpose (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        hstatus = transpose_hash(H1);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        double hash_trans_t = _delta_t_ns(t0, t1);
        hstatus = transpose_hash(H1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        printf("Hash scalar mul (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        hstatus = matrix_scalar_multiplication_hash(H1, H_scalar, 3.0f);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        double hash_scalar = _delta_t_ns(t0, t1);
        printf("Hash sum (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        hstatus = matrix_addition_hash(H1, H2, H_sum);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        double hash_sum_t = _delta_t_ns(t0, t1);
        printf("Hash mul (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        hstatus = matrix_multiplication_hash(H1, H2, H_mul);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        double hash_mul_t = _delta_t_ns(t0, t1);

        fprintf(timeExperimentsFile, "%d, %.12f, %d, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f\n",
                matrix_length, sparsity, k,
                dense_get_t, dense_set_t, dense_trans_t, dense_scalar, dense_sum_t, dense_mul_t,
                avl_get, avl_set, avl_trans, avl_scalar, avl_sum_t, avl_mul,
                hash_get_t, hash_set_t, hash_trans_t, hash_scalar, hash_sum_t, hash_mul_t);

        free_matrix_avl(A);
        free_matrix_avl(B);
        free_matrix_avl(scalar_out);
        free_matrix_avl(sum_out);
        free_matrix_avl(mul_out);
        free_hash_matrix(H1);
        free_hash_matrix(H2);
        free_hash_matrix(H_scalar);
        free_hash_matrix(H_sum);
        free_hash_matrix(H_mul);
        free(I);
        free(J);
        free(Data);
    }

    for(int experiment = NUM_DENSE_EXPERIMENTS; experiment < NUM_EXPERIMENTS; experiment++){ //Experimentos de tempo até o limite do denso
        int matrix_length = EXPERIMENT_MATRIX_LENGTH[experiment];
        float sparsity = EXPERIMENT_SPARSITY[experiment];
        struct timespec t0, t1;
        unsigned long long side = (unsigned long long)matrix_length;
        unsigned long long cells = side * side;
        int k = (int) ceil((double)cells * (double)sparsity);
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
        /* DENSE skipped: use sentinel -1 */
        double dense_get_t = -1.0;
        double dense_set_t = -1.0;
        double dense_trans_t = -1.0;
        double dense_scalar = -1.0;
        double dense_sum_t = -1.0;
        double dense_mul_t = -1.0;
        /* AVL */
        AVLMatrix* A = create_matrix_avl(matrix_length, matrix_length);
        AVLMatrix* B = create_matrix_avl(matrix_length, matrix_length);
        AVLMatrix* scalar_out = create_matrix_avl(matrix_length, matrix_length);
        AVLMatrix* sum_out = create_matrix_avl(matrix_length, matrix_length);
        AVLMatrix* mul_out = create_matrix_avl(matrix_length, matrix_length);
        if(!A || !B || !scalar_out || !sum_out || !mul_out){
            free(I);
            free(J);
            free(Data);
            if(A){free_matrix_avl(A);}
            if(B){free_matrix_avl(B);}
            if(scalar_out){free_matrix_avl(scalar_out);}
            if(sum_out){free_matrix_avl(sum_out);}
            if(mul_out){free_matrix_avl(mul_out);}
            fclose(timeExperimentsFile);
            return 1;
        }
        AVLStatus avlstatus = fill_avl_matrix(A, k, I, J, Data);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error filling AVL matrix (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        avlstatus = fill_avl_matrix(B, k, I, J, Data);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error filling AVL matrix B (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        int pos = rand() % k;
        int i = I[pos];
        int j = J[pos];
        float value;
        printf("AVL get (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = get_element_avl(A, i, j, &value);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error getting AVL at position (%d, %d) (status %d: %s).\n",
                    i,j,avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_get = _delta_t_ns(t0, t1);
        printf("AVL set (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = insert_element_avl(A, 3.14f ,i, j);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error inserting element on AVL at position (%d, %d) and value 3.14f (status %d: %s).\n",
                    i,j,avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_set = _delta_t_ns(t0, t1);
        printf("AVL transpose (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = transpose_avl(A);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error transposing AVL matrix (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_trans = _delta_t_ns(t0, t1);
        avlstatus = transpose_avl(A);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error transposing AVL matrix back (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }

        printf("AVL scalar mul (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = scalar_mul_avl(A, scalar_out, 3.0f);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error on AVL scalar multiplication (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_scalar = _delta_t_ns(t0, t1);

        printf("AVL sum (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = sum_avl(A, B, sum_out);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error on AVL sum (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_sum_t = _delta_t_ns(t0, t1);

        printf("AVL mul (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        avlstatus = matrix_mul_avl(A, B, mul_out);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(avlstatus != AVL_STATUS_OK){
            fprintf(stderr, "Error on AVL matrix multiplication (status %d: %s).\n",
                    avlstatus, avl_status_string(avlstatus));
            free(I);
            free(J);
            free(Data);
            free_matrix_avl(A);
            free_matrix_avl(B);
            free_matrix_avl(scalar_out);
            free_matrix_avl(sum_out);
            free_matrix_avl(mul_out);
            fclose(timeExperimentsFile);
            return 1;
        }
        double avl_mul = _delta_t_ns(t0, t1);

        /* HASH */
        printf("Hash setup (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        HashMatrix* H1 = create_hash_matrix(matrix_length, matrix_length);
        HashMatrix* H2 = create_hash_matrix(matrix_length, matrix_length);
        HashMatrix* H_scalar = create_hash_matrix(matrix_length, matrix_length);
        HashMatrix* H_sum = create_hash_matrix(matrix_length, matrix_length);
        HashMatrix* H_mul = create_hash_matrix(matrix_length, matrix_length);
        if(!H1 || !H2 || !H_scalar || !H_sum || !H_mul){
            _allocation_fail();
        }
        HashStatus hstatus = fill_hash_matrix(H1, k, I, J, Data);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        hstatus = fill_hash_matrix(H2, k, I, J, Data);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        int pos_h = rand() % k;
        int ih = I[pos_h];
        int jh = J[pos_h];
        printf("Hash get (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        float hval = get_element_hash(H1, ih, jh);
        (void)hval;
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double hash_get_t = _delta_t_ns(t0, t1);
        printf("Hash set (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        hstatus = set_element_hash(H1, ih, jh, 3.14f);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        double hash_set_t = _delta_t_ns(t0, t1);
        printf("Hash transpose (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        hstatus = transpose_hash(H1);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        double hash_trans_t = _delta_t_ns(t0, t1);
        hstatus = transpose_hash(H1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        printf("Hash scalar mul (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        hstatus = matrix_scalar_multiplication_hash(H1, H_scalar, 3.0f);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        double hash_scalar = _delta_t_ns(t0, t1);
        printf("Hash sum (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        hstatus = matrix_addition_hash(H1, H2, H_sum);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        double hash_sum_t = _delta_t_ns(t0, t1);
        printf("Hash mul (n=%d, sparsity=%.12f)\n", matrix_length, sparsity);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        hstatus = matrix_multiplication_hash(H1, H2, H_mul);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if(hstatus != HASH_STATUS_OK){
            _allocation_fail();
        }
        double hash_mul_t = _delta_t_ns(t0, t1);

        fprintf(timeExperimentsFile, "%d, %.12f, %d, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f\n",
                matrix_length, sparsity, k,
                dense_get_t, dense_set_t, dense_trans_t, dense_scalar, dense_sum_t, dense_mul_t,
                avl_get, avl_set, avl_trans, avl_scalar, avl_sum_t, avl_mul,
                hash_get_t, hash_set_t, hash_trans_t, hash_scalar, hash_sum_t, hash_mul_t);

        free_matrix_avl(A);
        free_matrix_avl(B);
        free_matrix_avl(scalar_out);
        free_matrix_avl(sum_out);
        free_matrix_avl(mul_out);
        free_hash_matrix(H1);
        free_hash_matrix(H2);
        free_hash_matrix(H_scalar);
        free_hash_matrix(H_sum);
        free_hash_matrix(H_mul);
        free(I);
        free(J);
        free(Data);
    }
    fclose(timeExperimentsFile);
    return 0;
}
