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

InnerNode* _insert_i(InnerNode* tree, int key, float value){
    if(!tree){
        InnerNode* new_node = malloc(sizeof (InnerNode));
        new_node -> key = key;
        new_node -> data = value;
        new_node -> left = NULL;
        new_node -> right = NULL;
        new_node -> height = 1;
        return new_node;
    }

    //TERMINAR DE IMPLEMENTAR AQUI

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

void insert_element_avl(AVLMatrix* matrix, float value, int i, int j){
    if(matrix -> is_transposed){
        int temp = i;
        i = j;
        j = i;
    }
    OuterNode* o_node = _find_node_o(matrix->root, i);
    if(o_node){
        //A linha não é vazia
        InnerNode* i_node = _find_node_i(o_node, j);
        if(i_node){
            i_node ->data = value; //Elemento já existe, atualizar valor
        }
        else{
            //Elemento não existe
        }
    }
    else{
        //A linha está vazia
    }
}