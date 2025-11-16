#include "AVL_Matrix.h"
#include <stdlib.h>
#include <stdio.h>

void _allocation_fail(){
    fprintf(stderr, "Error: memory allocation failed.\n");
    exit(EXIT_FAILURE);
}

const char* avl_status_string(AVLStatus status){
    switch(status){
        case AVL_STATUS_OK:
            return "Operation completed successfully";
        case AVL_STATUS_NOT_FOUND:
            return "Element not found";
        case AVL_ERROR_NULL_MATRIX:
            return "Matrix pointer is NULL";
        case AVL_ERROR_OUT_OF_BOUNDS:
            return "Indices out of bounds";
        case AVL_ERROR_DIMENSION_MISMATCH:
            return "Matrix dimensions mismatch";
        case AVL_ERROR_INVALID_ARGUMENT:
            return "Invalid argument";
        case AVL_ERROR_NOT_IMPLEMENTED:
            return "Operation not implemented";
        default:
            return "Unknown error";
    }
}

static AVLStatus _validate_matrix(AVLMatrix* matrix){
    if(!matrix){
        return AVL_ERROR_NULL_MATRIX;
    }
    if(matrix->n < 0 || matrix->m < 0){
        return AVL_ERROR_INVALID_ARGUMENT;
    }
    return AVL_STATUS_OK;
}

static AVLStatus _validate_indices(AVLMatrix* matrix, int i, int j){
    AVLStatus status = _validate_matrix(matrix);
    if(status != AVL_STATUS_OK){
        return status;
    }
    if(i < 0 || i >= matrix->n || j < 0 || j >= matrix->m){
        return AVL_ERROR_OUT_OF_BOUNDS;
    }
    return AVL_STATUS_OK;
}

static AVLStatus _validate_same_dimensions(AVLMatrix* A, AVLMatrix* B){
    AVLStatus status = _validate_matrix(A);
    if(status != AVL_STATUS_OK){
        return status;
    }
    status = _validate_matrix(B);
    if(status != AVL_STATUS_OK){
        return status;
    }
    if(A->n != B->n || A->m != B->m){
        return AVL_ERROR_DIMENSION_MISMATCH;
    }
    return AVL_STATUS_OK;
}

int _height_i(InnerNode* tree){
    if(!tree){
        return 0; 
    }
    return tree->height;
}
int _height_o(OuterNode* tree){
    if(!tree){
        return 0; 
    }
    return tree->height;
}
int _balance_factor_i(InnerNode* tree){
    return _height_i(tree->left) - _height_i(tree->right);
}
int _balance_factor_o(OuterNode* tree){
    return _height_o(tree->left) - _height_o(tree->right);
}

int _max(int a, int b) {
    return (a > b) ? a : b;
}

InnerNode* _left_rotate_i(InnerNode* tree){
    InnerNode* right = tree->right;
    tree -> right = right->left;
    right->left = tree;

    tree->height  = 1 + _max(_height_i(tree->left),  _height_i(tree->right));
    right->height = 1 + _max(_height_i(right->left), _height_i(right->right));
    return right;
}

InnerNode* _right_rotate_i(InnerNode* tree){
    InnerNode* left = tree->left;
    tree -> left = left-> right;
    left->right = tree;

    tree->height = 1 + _max(_height_i(tree->left), _height_i(tree->right));
    left->height = 1 + _max(_height_i(left->left), _height_i(left->right));
    return left;
}

OuterNode* _left_rotate_o(OuterNode* tree){
    OuterNode* right = tree->right;
    tree->right = right->left;
    right->left = tree;

    tree->height  = 1 + _max(_height_o(tree->left),  _height_o(tree->right));
    right->height = 1 + _max(_height_o(right->left), _height_o(right->right));

    return right;
}

OuterNode* _right_rotate_o(OuterNode* tree){
    OuterNode* left = tree->left;
    tree->left = left->right;
    left->right = tree;

    tree->height = 1 + _max(_height_o(tree->left), _height_o(tree->right));
    left->height = 1 + _max(_height_o(left->left), _height_o(left->right));

    return left;
}

InnerNode* _find_node_i(InnerNode* tree, int search_key){
    if(!tree){
        return NULL;
    }
    if(tree->key == search_key){
        return tree;
    }
    if(tree -> key < search_key){
        return _find_node_i(tree -> right, search_key);
    }
    else{
        return _find_node_i(tree -> left, search_key);
    }
}

OuterNode* _find_node_o(OuterNode* tree, int search_key){
    if(!tree){
        return NULL;
    }
    if(tree->key == search_key){
        return tree;
    }
    if(tree -> key < search_key){
        return _find_node_o(tree -> right, search_key);
    }
    else{
        return _find_node_o(tree -> left, search_key);
    }
}

InnerNode* _insert_i(InnerNode* tree, int insert_key, float value, int* already_existed){
    if(!tree){
        InnerNode* new_node = malloc(sizeof (InnerNode));
        if(!new_node){
            _allocation_fail();
        }
        new_node -> key = insert_key;
        new_node -> data = value;
        new_node -> left = NULL;
        new_node -> right = NULL;
        new_node -> height = 1;
        return new_node;
    }

    if(tree->key == insert_key){
        *already_existed = 1;
        tree -> data = value;
        return tree;
    }
    if(tree->key < insert_key){
        tree -> right = _insert_i(tree->right, insert_key, value, already_existed);
    }
    if(tree->key > insert_key){
        tree -> left = _insert_i(tree->left, insert_key, value, already_existed);
    }

    tree->height = 1 + _max(_height_i(tree->left), _height_i(tree->right));

    int bf = _balance_factor_i(tree);

    if(bf > 1){//Sub-árvore esquerda grande
        int sub_bf = _balance_factor_i(tree->left);
        if(sub_bf >= 0){//Causa é a sub-árvore esquerda do filho esquerdo: LL
            return _right_rotate_i(tree);
        }
        else{//Causa é a sub-árvore direita do filho esquerdo: LR
            tree -> left = _left_rotate_i(tree -> left);
            return _right_rotate_i(tree);
        }
    }
    if(bf < -1){//Sub-árvore direita grande
        int sub_bf = _balance_factor_i(tree->right);
        if(sub_bf < 0){//Causa é a sub-árvore direita do filho direito: RR
            return _left_rotate_i(tree);
        }
        else{//Causa é a sub-árvore esquerda do filho direito: RL
            tree -> right = _right_rotate_i(tree -> right);
            return _left_rotate_i(tree);
        }
    }

    return tree;

}

OuterNode* _insert_o(OuterNode* tree, int insert_key, InnerNode* inner_tree){
    if(!tree){
        OuterNode* new_node = malloc(sizeof (OuterNode));
        if(!new_node){
            _allocation_fail();
        }
        new_node -> key = insert_key;
        new_node -> inner_tree = inner_tree;
        new_node -> left = NULL;
        new_node -> right = NULL;
        new_node -> height = 1;
        return new_node;
    }

    if(tree->key == insert_key){
        tree -> inner_tree = inner_tree;
        return tree;
    }
    if(tree->key < insert_key){
        tree -> right = _insert_o(tree->right, insert_key, inner_tree);
    }
    if(tree->key > insert_key){
        tree -> left = _insert_o(tree->left, insert_key, inner_tree);
    }

    tree -> height = 1 + _max(_height_o(tree -> left), _height_o(tree -> right));

    int bf = _balance_factor_o(tree);

    if(bf > 1){//Sub-árvore esquerda grande
        int sub_bf = _balance_factor_o(tree->left);
        if(sub_bf >= 0){//Causa é a sub-árvore esquerda do filho esquerdo: LL
            return _right_rotate_o(tree);
        }
        else{//Causa é a sub-árvore direita do filho esquerdo: LR
            tree -> left = _left_rotate_o(tree -> left);
            return _right_rotate_o(tree);
        }
    }
    if(bf < -1){//Sub-árvore direita grande
        int sub_bf = _balance_factor_o(tree->right);
        if(sub_bf < 0){//Causa é a sub-árvore direita do filho direito: RR
            return _left_rotate_o(tree);
        }
        else{//Causa é a sub-árvore esquerda do filho direito: RL
            tree -> right = _right_rotate_o(tree -> right);
            return _left_rotate_o(tree);
        }
    }

    return tree;

}
InnerNode * _find_max_i (InnerNode * tree){
    if(tree == NULL){
        return NULL;
    }
    if(tree -> right == NULL){
        return tree;
    }
    return _find_max_i(tree -> right);
}

OuterNode * _find_max_o (OuterNode * tree){
    if(tree == NULL){
        return NULL;
    }
    if(tree -> right == NULL){
        return tree;
    }
    return _find_max_o(tree -> right);
}

InnerNode * _remove_i(InnerNode* tree, int remove_key){
    if(tree == NULL){
        return NULL;
    }
    if(tree -> key < remove_key){
        tree->right = _remove_i(tree->right, remove_key);
    }
    else if(tree->key > remove_key){
        tree->left = _remove_i(tree->left, remove_key);
    }
    else{
        if(tree->left == NULL){ //Sem filho esquerdo ou sem filhos
            InnerNode* right = tree->right;
            free(tree);
            return right; //No caso sem filhos, retorna NULL
        }
        else if(tree->right == NULL){//Sem filho direito
            InnerNode* left = tree->left;
            free(tree);
            return left;
        }
        else{ //Dois filhos
            InnerNode* max_of_left = _find_max_i(tree->left);
            tree->key = max_of_left->key;
            tree->data = max_of_left->data;

            tree->left = _remove_i(tree->left, max_of_left->key);
        }
    }
    if(tree == NULL){//Se a remoção esvaziou essa sub-árvore, nada a fazer.
        return NULL;
    }

    tree->height = 1 + _max(_height_i(tree->left), _height_i(tree->right));

    int bf = _balance_factor_i(tree);

    if(bf > 1){//Sub-árvore esquerda grande
        int sub_bf = _balance_factor_i(tree->left);
        if(sub_bf >= 0){//Causa é a sub-árvore esquerda do filho esquerdo: LL
            return _right_rotate_i(tree);
        }
        else{//Causa é a sub-árvore direita do filho esquerdo: LR
            tree -> left = _left_rotate_i(tree -> left);
            return _right_rotate_i(tree);
        }
    }
    if(bf < -1){//Sub-árvore direita grande
        int sub_bf = _balance_factor_i(tree->right);
        if(sub_bf < 0){//Causa é a sub-árvore direita do filho direito: RR
            return _left_rotate_i(tree);
        }
        else{//Causa é a sub-árvore esquerda do filho direito: RL
            tree -> right = _right_rotate_i(tree -> right);
            return _left_rotate_i(tree);
        }
    }

    return tree;

}

OuterNode * _remove_o(OuterNode* tree, int remove_key){
    if(tree == NULL){
        return NULL;
    }
    if(tree -> key < remove_key){
        tree->right = _remove_o(tree->right, remove_key);
    }
    else if(tree->key > remove_key){
        tree->left = _remove_o(tree->left, remove_key);
    }
    else{
        if(tree->left == NULL){
            OuterNode* right = tree->right;
            free(tree);
            return right;
        }
        else if(tree->right == NULL){
            OuterNode* left = tree->left;
            free(tree);
            return left;
        }
        else{
            OuterNode* max_of_left = _find_max_o(tree->left);
            tree->key = max_of_left->key;
            tree->inner_tree = max_of_left->inner_tree;

            tree->left = _remove_o(tree->left, max_of_left->key);
        }
    }

    if(tree == NULL){
        return NULL;
    }

    tree->height = 1 + _max(_height_o(tree->left), _height_o(tree->right));

    int bf = _balance_factor_o(tree);

    if(bf > 1){
        int sub_bf = _balance_factor_o(tree->left);
        if(sub_bf >= 0){
            return _right_rotate_o(tree);
        }
        else{
            tree -> left = _left_rotate_o(tree -> left);
            return _right_rotate_o(tree);
        }
    }
    if(bf < -1){
        int sub_bf = _balance_factor_o(tree->right);
        if(sub_bf < 0){
            return _left_rotate_o(tree);
        }
        else{
            tree -> right = _right_rotate_o(tree -> right);
            return _left_rotate_o(tree);
        }
    }

    return tree;
}

void _free_i_tree(InnerNode* tree){
    if(tree == NULL){
        return;
    }
    _free_i_tree(tree->left);
    _free_i_tree(tree->right);
    free(tree);
    return;
}

void _free_o_tree(OuterNode* tree){
    if(tree == NULL){
        return;
    }
    _free_o_tree(tree->left);
    _free_o_tree(tree->right);
    _free_i_tree(tree->inner_tree);
    free(tree);
    return;
}

InnerNode* _clone_inner_tree(InnerNode* tree){
    if(!tree){
        return NULL;
    }
    InnerNode* new_node = malloc(sizeof(InnerNode));
    if(!new_node){
        _allocation_fail();
    }
    new_node->key = tree->key;
    new_node->data = tree->data;
    new_node->height = tree->height;
    new_node->left = _clone_inner_tree(tree->left);
    new_node->right = _clone_inner_tree(tree->right);
    return new_node;
}

OuterNode* _clone_outer_tree(OuterNode* tree){
    if(!tree){
        return NULL;
    }
    OuterNode* new_node = malloc(sizeof(OuterNode));
    if(!new_node){
        _allocation_fail();
    }
    new_node->key = tree->key;
    new_node->height = tree->height;
    new_node->inner_tree = _clone_inner_tree(tree->inner_tree);
    new_node->left = _clone_outer_tree(tree->left);
    new_node->right = _clone_outer_tree(tree->right);
    return new_node;
}

AVLStatus _copy_matrix(AVLMatrix* source, AVLMatrix* dest){
    if(!source || !dest){
        return AVL_ERROR_NULL_MATRIX;
    }
    if(source == dest){
        return AVL_STATUS_OK;
    }
    AVLStatus status = _validate_matrix(source);
    if(status != AVL_STATUS_OK){
        return status;
    }
    _free_o_tree(dest->main_root);
    _free_o_tree(dest->transposed_root);
    dest->main_root = _clone_outer_tree(source->main_root);
    dest->transposed_root = _clone_outer_tree(source->transposed_root);
    dest->k = source->k;
    dest->n = source->n;
    dest->m = source->m;
    return AVL_STATUS_OK;
}

void _multiply_inner_tree(InnerNode* tree, float a){
    if(!tree){
        return;
    }
    _multiply_inner_tree(tree->left, a);
    tree->data = tree->data * a;
    _multiply_inner_tree(tree->right, a);
}

void _multiply_outer_tree(OuterNode* tree, float a){
    if(!tree){
        return;
    }
    _multiply_outer_tree(tree->left, a);
    _multiply_inner_tree(tree->inner_tree, a);
    _multiply_outer_tree(tree->right, a);
}

void _copy_inner(InnerNode* inner_tree, int* I, int* J, float* Data, int* position, int i){
    if(!inner_tree){
        return;
    }

    _copy_inner(inner_tree -> left, I, J, Data, position, i);
    I[*position] = i;
    J[*position] = inner_tree->key;
    Data[*position] = inner_tree->data;
    *position = *position + 1;
    _copy_inner(inner_tree -> right, I, J, Data, position, i);
    return;
}

void _copy_outer(OuterNode* outer_tree, int* I, int* J, float* Data, int* position){
    if(!outer_tree){
        return;
    }

    _copy_outer(outer_tree -> left, I, J, Data, position);
    _copy_inner(outer_tree->inner_tree, I, J, Data, position, outer_tree->key);
    _copy_outer(outer_tree-> right, I, J, Data, position);
    return;
}

AVLStatus get_element_avl(AVLMatrix* matrix, int i, int j, float* out_value){
    if(!out_value){
        return AVL_ERROR_INVALID_ARGUMENT;
    }
    *out_value = 0.0f;
    AVLStatus status = _validate_indices(matrix, i, j);
    if(status != AVL_STATUS_OK){
        return status;
    }
    OuterNode* o_node = _find_node_o(matrix->main_root, i);
    if(!o_node){
        return AVL_STATUS_OK;
    }
    InnerNode* i_node = _find_node_i(o_node->inner_tree, j);
    if(!i_node){
        return AVL_STATUS_OK;
    }
    *out_value = i_node -> data;
    return AVL_STATUS_OK;
}

AVLStatus insert_element_avl(AVLMatrix* matrix, float value, int i, int j){
    AVLStatus status = _validate_indices(matrix, i, j);
    if(status != AVL_STATUS_OK){
        return status;
    }
    int already_existed = 0;
    OuterNode* o_node = _find_node_o(matrix->main_root, i);
    if(o_node){
        o_node->inner_tree = _insert_i(o_node->inner_tree, j, value, &already_existed);
    }
    else{
        InnerNode* new_main_i_tree = _insert_i(NULL, j, value, &already_existed);
        matrix -> main_root = _insert_o(matrix->main_root, i, new_main_i_tree);
    }
    if(!already_existed){
        matrix-> k = matrix -> k + 1;
    }

    int transposed_existed = already_existed;
    OuterNode* o_node_transposed = _find_node_o(matrix->transposed_root, j);
    if(o_node_transposed){
        o_node_transposed->inner_tree = _insert_i(o_node_transposed->inner_tree, i, value, &transposed_existed);
    }
    else{
        InnerNode* new_transposed_i_tree = _insert_i(NULL, i, value, &transposed_existed);
        matrix -> transposed_root = _insert_o(matrix->transposed_root, j, new_transposed_i_tree);
    }
    return AVL_STATUS_OK;
}

AVLStatus delete_element_avl(AVLMatrix* matrix, int i, int j){
    AVLStatus status = _validate_indices(matrix, i, j);
    if(status != AVL_STATUS_OK){
        return status;
    }

    OuterNode* o_node_main = _find_node_o(matrix->main_root, i);
    if(!o_node_main){
        return AVL_STATUS_NOT_FOUND;
    }
    InnerNode* i_node_main = _find_node_i(o_node_main->inner_tree, j);
    if(!i_node_main){
        return AVL_STATUS_NOT_FOUND;
    }

    o_node_main->inner_tree = _remove_i(o_node_main->inner_tree, j);
    matrix -> k = matrix -> k - 1;

    if(o_node_main->inner_tree == NULL){
        matrix->main_root = _remove_o(matrix->main_root, i);
    }

    OuterNode* o_node_transposed = _find_node_o(matrix->transposed_root, j);
    if(!o_node_transposed){
        return AVL_ERROR_INVALID_ARGUMENT;
    }
    o_node_transposed->inner_tree = _remove_i(o_node_transposed->inner_tree, i);
    if(o_node_transposed->inner_tree == NULL){
        matrix->transposed_root = _remove_o(matrix->transposed_root, j);
    }

    return AVL_STATUS_OK;
}

AVLStatus transpose_avl(AVLMatrix* matrix){
    AVLStatus status = _validate_matrix(matrix);
    if(status != AVL_STATUS_OK){
        return status;
    }
    OuterNode* temp = matrix -> main_root;
    matrix -> main_root = matrix -> transposed_root;
    matrix -> transposed_root = temp;
    int temp_dim = matrix->n;
    matrix->n = matrix->m;
    matrix->m = temp_dim;
    return AVL_STATUS_OK;
}

AVLStatus scalar_mul_avl(AVLMatrix* A, AVLMatrix* B, float a){
    if(!A || !B){
        return AVL_ERROR_NULL_MATRIX;
    }

    if(A != B){
        AVLStatus status = _validate_same_dimensions(A, B);
        if(status != AVL_STATUS_OK){
            return status;
        }
    } else {
        AVLStatus status = _validate_matrix(A);
        if(status != AVL_STATUS_OK){
            return status;
        }
    }

    if(A == B){
        if(a == 0.0f){
            _free_o_tree(A->main_root);
            _free_o_tree(A->transposed_root);
            A->main_root = NULL;
            A->transposed_root = NULL;
            A->k = 0;
            return AVL_STATUS_OK;
        }
        _multiply_outer_tree(A->main_root, a);
        _multiply_outer_tree(A->transposed_root, a);
        return AVL_STATUS_OK;
    }

    if(a == 0.0f){
        _free_o_tree(B->main_root);
        _free_o_tree(B->transposed_root);
        B->main_root = NULL;
        B->transposed_root = NULL;
        B->k = 0;
        B->n = A->n;
        B->m = A->m;
        return AVL_STATUS_OK;
    }

    AVLStatus status = _copy_matrix(A, B);
    if(status != AVL_STATUS_OK){
        return status;
    }
    _multiply_outer_tree(B->main_root, a);
    _multiply_outer_tree(B->transposed_root, a);
    return AVL_STATUS_OK;
}

AVLStatus sum_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C){
    if(!A || !B || !C){
        return AVL_ERROR_NULL_MATRIX;
    }
    AVLStatus status = _validate_same_dimensions(A, B);
    if(status != AVL_STATUS_OK){
        return status;
    }
    status = _validate_same_dimensions(A, C);
    if(status != AVL_STATUS_OK){
        return status;
    }

    status = _copy_matrix(B, C);
    if(status != AVL_STATUS_OK){
        return status;
    }
    int* I = NULL;
    int* J = NULL;
    float* Data = NULL;
    if(A->k > 0){
        I = (int*) malloc(sizeof(int) * A->k);
        J = (int*) malloc(sizeof(int) * A->k);
        Data = (float*) malloc(sizeof(float) * A-> k);
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
    }
    int position = 0;
    _copy_outer(A->main_root, I, J, Data, &position);
    for(int pos = 0; pos < A->k; pos++){
        float element = 0.0f;
        status = get_element_avl(C, I[pos], J[pos], &element);
        if(status != AVL_STATUS_OK){
            free(I);
            free(J);
            free(Data);
            return status;
        }
        status = insert_element_avl(C, element + Data[pos], I[pos], J[pos]);
        if(status != AVL_STATUS_OK){
            free(I);
            free(J);
            free(Data);
            return status;
        }
    }

    free(I);
    free(J);
    free(Data);
    return AVL_STATUS_OK;
}
AVLStatus matrix_mul_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C){
    if(!A || !B || !C){
        return AVL_ERROR_NULL_MATRIX;
    }
    if(!(A-> n == B -> m)){
        return AVL_ERROR_DIMENSION_MISMATCH;
    }
    if(C->n != A->n || C-> m != B-> m){
        return AVL_ERROR_DIMENSION_MISMATCH;
    }
    if(A == C || B == C){ //Não vamos implementar multiplicação de matrizes "in-place" no momento
        return AVL_ERROR_NOT_IMPLEMENTED;
    }
    _free_o_tree(C->main_root);
    _free_o_tree(C->transposed_root);
    C-> main_root = NULL;
    C-> transposed_root = NULL;
    C-> k = 0;

    
    return AVL_ERROR_NOT_IMPLEMENTED; //TODO
}
AVLMatrix* create_matrix_avl(int n, int m){
    if(n < 0 || m < 0){
        fprintf(stderr, "Error: matrix dimensions must be non-negative.\n");
        return NULL;
    }
    AVLMatrix* matrix = malloc(sizeof(AVLMatrix));
    if(!matrix){
        _allocation_fail();
    }
    matrix->main_root = NULL;
    matrix->transposed_root = NULL;
    matrix->k = 0;
    matrix->n = n;
    matrix->m = m;
    return matrix;
}
void free_matrix_avl(AVLMatrix* matrix){
    if(!matrix){
        return;
    }
    _free_o_tree(matrix->main_root);
    _free_o_tree(matrix->transposed_root);
    free(matrix);
}
