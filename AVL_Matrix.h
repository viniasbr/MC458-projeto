#pragma once

typedef struct InnerNode{
    int key;
    float data;
    struct InnerNode* left;
    struct InnerNode* right;
    int height;
} InnerNode;

typedef struct OuterNode{
    int key;
    struct InnerNode* inner_tree; 
    struct OuterNode* left;
    struct OuterNode* right;
    int height;
} OuterNode;

typedef enum {
    AVL_STATUS_OK = 0,
    AVL_STATUS_NOT_FOUND = 1,
    AVL_ERROR_NULL_MATRIX = -1,
    AVL_ERROR_OUT_OF_BOUNDS = -2,
    AVL_ERROR_DIMENSION_MISMATCH = -3,
    AVL_ERROR_INVALID_ARGUMENT = -4,
    AVL_ERROR_NOT_IMPLEMENTED = -5
} AVLStatus;

typedef struct AVLMatrix{
    struct OuterNode* main_root;
    struct OuterNode* transposed_root;
    int k;
    int n;
    int m;
} AVLMatrix;

AVLStatus get_element_avl(AVLMatrix* matrix, int i, int j, float* out_value);
AVLStatus insert_element_avl(AVLMatrix* matrix, float value, int i, int j);
AVLStatus delete_element_avl(AVLMatrix* matrix, int i, int j);
AVLStatus transpose_avl(AVLMatrix* matrix);
AVLStatus scalar_mul_avl(AVLMatrix* A, AVLMatrix* B, float a);
AVLStatus sum_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C);
AVLStatus matrix_mul_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C);

const char* avl_status_string(AVLStatus status);

AVLMatrix* create_matrix_avl(int n, int m);
void free_matrix_avl(AVLMatrix* matrix);
