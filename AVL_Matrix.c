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
    return height_i(tree->left) - height_i(tree->right);
}
int _balance_factor_o(OuterNode* tree){
    return height_o(tree->left) - height_o(tree->right);
}

InnerNode* _left_rotate_i(InnerNode* tree){
    InnerNode* right = tree->right;
    tree -> right = right->left;
    right->left = tree;
    return right;
}

InnerNode* _right_rotate_i(InnerNode* tree){
    InnerNode* left = tree->left;
    tree -> left = left-> right;
    left->right = tree;
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

float get_element_avl(AVLMatrix* matrix, int i, int j){
    if(matrix -> is_transposed){
        int temp = i;
        i = j;
        j = i;
    }
    OuterNode* o_node = _find_node_o(matrix->root, i);
    InnerNode* i_node = _find_node_i(o_node, j);
    if(!i_node){
        return NAN;
    }
    return i_node -> data;
}