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

typedef struct AVLMatrix{
    unsigned char is_transposed;
    struct OuterNode* root;
    int k;
} AVLMatrix;

float get_element_avl(AVLMatrix* matrix, int i, int j);
void insert_element_avl(AVLMatrix* matrix, float value, int i, int j);
int delete_element_avl(AVLMatrix* matrix, int i, int j);
void transpose_avl(AVLMatrix* matrix);
void scalar_mul_avl(AVLMatrix* A, AVLMatrix* B, int a);
void sum_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C);
void matrix_mul_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C);
AVLMatrix* create_matrix_avl();
void free_matrix_avl(AVLMatrix* matrix);