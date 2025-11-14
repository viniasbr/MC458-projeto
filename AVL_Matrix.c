#include "AVL_Matrix.h"
#include <stdlib.h>
#include <math.h>

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
        return _find_node_i(tree -> left, search_key);
    }
    else{
        return _find_node_i(tree -> right, search_key);
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
        return _find_node_o(tree -> left, search_key);
    }
    else{
        return _find_node_o(tree -> right, search_key);
    }
}

InnerNode* _insert_i(InnerNode* tree, int insert_key, float value){
    if(!tree){
        InnerNode* new_node = malloc(sizeof (InnerNode));
        new_node -> key = insert_key;
        new_node -> data = value;
        new_node -> left = NULL;
        new_node -> right = NULL;
        new_node -> height = 1;
        return new_node;
    }

    if(tree->key == insert_key){
        tree -> data = value;
        return tree;
    }
    if(tree->key < insert_key){
        tree -> left = _insert_i(tree->left, insert_key, value);
    }
    if(tree->key > insert_key){
        tree -> right = _insert_i(tree->right, insert_key, value);
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
        tree -> left = _insert_o(tree->left, insert_key, inner_tree);
    }
    if(tree->key > insert_key){
        tree -> right = _insert_o(tree->right, insert_key, inner_tree);
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
    return;
}

float get_element_avl(AVLMatrix* matrix, int i, int j){
    if(matrix -> is_transposed){
        int temp = i;
        i = j;
        j = temp;
    }
    OuterNode* o_node = _find_node_o(matrix->root, i);
    if(!o_node){
        return NAN;
    }
    InnerNode* i_node = _find_node_i(o_node->inner_tree, j);
    if(!i_node){
        return NAN;
    }
    return i_node -> data;
}

void insert_element_avl(AVLMatrix* matrix, float value, int i, int j){
    if(matrix -> is_transposed){
        int temp = i;
        i = j;
        j = temp;
    }
    OuterNode* o_node = _find_node_o(matrix->root, i);
    if(o_node){
        o_node->inner_tree = _insert_i(o_node->inner_tree, j, value);
        return;
    }
    else{
        InnerNode* new_i_tree = _insert_i(NULL, j, value);
        matrix -> root = _insert_o(matrix->root, i, new_i_tree);
        return;
    }
}

int delete_element_avl(AVLMatrix* matrix, int i, int j); //TODO

void transpose_avl(AVLMatrix* matrix){
    matrix->is_transposed = !matrix->is_transposed;
}

void sum_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C); //TODO
void scalar_mul_avl(AVLMatrix* A, AVLMatrix* B, int a); //TODO
void matrix_mul_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C); //TODO
AVLMatrix* create_matrix_avl(){
    AVLMatrix* matrix = malloc(sizeof(AVLMatrix));
    matrix->root = NULL;
    matrix->is_transposed = 0;
    return matrix;
}
void free_matrix_avl(AVLMatrix* matrix){
    _free_o_tree(matrix->root);
    free(matrix);
}