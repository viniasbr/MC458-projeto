#include "AVL_Matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void _allocation_fail(){
    fprintf(stderr, "Error: memory allocation failed.\n");
    exit(EXIT_FAILURE);
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

void _copy_matrix(AVLMatrix* source, AVLMatrix* dest){
    if(!source || !dest){
        return;
    }
    if(source == dest){
        return;
    }
    _free_o_tree(dest->main_root);
    _free_o_tree(dest->transposed_root);
    dest->main_root = _clone_outer_tree(source->main_root);
    dest->transposed_root = _clone_outer_tree(source->transposed_root);
    dest->k = source->k;
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

float get_element_avl(AVLMatrix* matrix, int i, int j){
    if(!matrix){
        return NAN;
    }
    OuterNode* o_node = _find_node_o(matrix->main_root, i);
    if(!o_node){
        return 0.0f;
    }
    InnerNode* i_node = _find_node_i(o_node->inner_tree, j);
    if(!i_node){
        return 0.0f;
    }
    return i_node -> data;
}

void insert_element_avl(AVLMatrix* matrix, float value, int i, int j){
    if(!matrix){
        return;
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
    return;
}

int delete_element_avl(AVLMatrix* matrix, int i, int j){
    if(!matrix){
        return -1;
    }

    OuterNode* o_node_main = _find_node_o(matrix->main_root, i);
    if(!o_node_main){
        return 0;
    }
    InnerNode* i_node_main = _find_node_i(o_node_main->inner_tree, j);
    if(!i_node_main){
        return 0;
    }

    o_node_main->inner_tree = _remove_i(o_node_main->inner_tree, j);
    matrix -> k = matrix -> k - 1;

    if(o_node_main->inner_tree == NULL){
        matrix->main_root = _remove_o(matrix->main_root, i);
    }

    OuterNode* o_node_transposed = _find_node_o(matrix->transposed_root, j);
    if(o_node_transposed){
        o_node_transposed->inner_tree = _remove_i(o_node_transposed->inner_tree, i);
        if(o_node_transposed->inner_tree == NULL){
            matrix->transposed_root = _remove_o(matrix->transposed_root, j);
        }
    }
    else{//Se estiver na árvore main mas não na árvore transposta, algo deu errado.
        return -1;
    }

    return 1;
}

void transpose_avl(AVLMatrix* matrix){
    if(!matrix){
        return;
    }
    OuterNode* temp = matrix -> main_root;
    matrix -> main_root = matrix -> transposed_root;
    matrix -> transposed_root = temp;
    return;
}

void scalar_mul_avl(AVLMatrix* A, AVLMatrix* B, float a){
    if(!A || !B){
        return;
    }

    if(A == B){
        if(a == 0.0f){
            _free_o_tree(A->main_root);
            _free_o_tree(A->transposed_root);
            A->main_root = NULL;
            A->transposed_root = NULL;
            A->k = 0;
            return;
        }
        _multiply_outer_tree(A->main_root, a);
        _multiply_outer_tree(A->transposed_root, a);
        return;
    }

    if(a == 0.0f){
        _free_o_tree(B->main_root);
        _free_o_tree(B->transposed_root);
        B->main_root = NULL;
        B->transposed_root = NULL;
        B->k = 0;
        return;
    }

    _copy_matrix(A, B);
    _multiply_outer_tree(B->main_root, a);
    _multiply_outer_tree(B->transposed_root, a);
    return;
}

void sum_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C){
    if(!A || !B || !C){
        return;
    }
    _copy_matrix(B, C);
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
        float element = get_element_avl(C, I[pos], J[pos]);
        insert_element_avl(C, element + Data[pos], I[pos], J[pos]);
    }

    free(I);
    free(J);
    free(Data);
}
void matrix_mul_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C); //TODO
AVLMatrix* create_matrix_avl(){
    AVLMatrix* matrix = malloc(sizeof(AVLMatrix));
    if(!matrix){
        _allocation_fail();
    }
    matrix->main_root = NULL;
    matrix->transposed_root = NULL;
    matrix->k = 0;
    return matrix;
}
void free_matrix_avl(AVLMatrix* matrix){
    _free_o_tree(matrix->main_root);
    _free_o_tree(matrix->transposed_root);
    free(matrix);
}
