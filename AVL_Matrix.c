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

void _scalar_mul_inner(InnerNode* inner_tree, AVLMatrix* B, int i, int a){
    if(!inner_tree){
        return;
    }

    _scalar_mul_inner(inner_tree -> left, B, i, a);

    int row = i;
    int col = inner_tree->key;
    if(B->is_transposed){
        int temp = row;
        row = col;
        col = temp;
    }
    insert_element_avl(B, inner_tree->data * a, row, col);

    _scalar_mul_inner(inner_tree -> right, B, i, a);
    return;
}

void _scalar_mul_outer(OuterNode* outer_tree, AVLMatrix* B, int a){
    if(!outer_tree){
        return;
    }

    _scalar_mul_outer(outer_tree -> left, B, a);

    _scalar_mul_inner(outer_tree-> inner_tree, B, outer_tree-> key, a);

    _scalar_mul_outer(outer_tree -> right, B, a);
    return;
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
    if(matrix -> is_transposed){
        int temp = i;
        i = j;
        j = temp;
    }
    OuterNode* o_node = _find_node_o(matrix->root, i);
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
    if(matrix -> is_transposed){
        int temp = i;
        i = j;
        j = temp;
    }
    int already_existed = 0;
    OuterNode* o_node = _find_node_o(matrix->root, i);
    if(o_node){
        o_node->inner_tree = _insert_i(o_node->inner_tree, j, value, &already_existed);
        if(!already_existed){
            matrix-> k = matrix -> k + 1;
        }
        return;
    }
    else{
        InnerNode* new_i_tree = _insert_i(NULL, j, value, &already_existed);
        matrix -> root = _insert_o(matrix->root, i, new_i_tree);
        matrix -> k = matrix -> k + 1;
        return;
    }
}

int delete_element_avl(AVLMatrix* matrix, int i, int j){
    if(!matrix){
        return -1;
    }

    if(matrix->is_transposed){
        int temp = i;
        i = j;
        j = temp;
    }

    OuterNode* o_node = _find_node_o(matrix->root, i);
    if(!o_node){
        //Linha não existe
        return 0;
    }
    InnerNode* i_node = _find_node_i(o_node->inner_tree, j);
    if(!i_node){
        //Dado não existe
        return 0;
    }

    o_node->inner_tree = _remove_i(o_node->inner_tree, j);
    matrix -> k = matrix -> k - 1;

    if(o_node->inner_tree == NULL){
        //Se a árvore interna de um nó externo está vazia, o nó externo não existe.
        matrix->root = _remove_o(matrix->root, i);
    }

    return 1;
}

void transpose_avl(AVLMatrix* matrix){
    if(!matrix){
        return;
    }
    matrix->is_transposed = !matrix->is_transposed;
}

void scalar_mul_avl(AVLMatrix* A, AVLMatrix* B, int a){
    if(!A || !B){
        return;
    }

    OuterNode* source_tree = A->root;
    int same_matrix = (A == B);
    if(!same_matrix){ //Limpa a variável que vai usar caso não seja a mesma matriz.
        _free_o_tree(B->root);
    }
    B-> root = NULL; //Se for a mesma matriz, o ponteiro para os dados está salvo.
    B-> k = 0; //_scalar_mul_outer faz a contagem novamente.

    B->is_transposed = A->is_transposed;

    if(a == 0){
        if(same_matrix){
            _free_o_tree(source_tree); //Se multiplicar a própria matriz por zero, deve-se apagar tudo.
        }
        return;
    }

    _scalar_mul_outer(source_tree, B, a);

    if(same_matrix){
        _free_o_tree(source_tree); //A árvore original pode ser liberada.
    }

    return;
}

void sum_avl(AVLMatrix* A, AVLMatrix* B, AVLMatrix* C){
    /*Ideia para implementação: Copiar B para C, armazenar os valores de A em uma pilha ou fila, desempilhar ou desenfileirar
    conferindo se o valor está em C. Se tiver, atualizar o valor para o valor somado. Caso não esteja, inserir o valor em C.
    Dá pra usar uma pilha de tamanho constante, uma vez que agora a matriz sabe a quantidade de elementos não nulos dentro dela.*/
    if(!A || !B || !C){
        return;
    }
    scalar_mul_avl(B, C, 1);
    int* I = (int*) malloc(sizeof(int) * A->k);
    int* J = (int*) malloc(sizeof(int) * A->k);
    float* Data = (float*) malloc(sizeof(float) * A-> k);
    int position = 0;
    _copy_outer(A->root, I, J, Data, &position);
    if(A->is_transposed){
        int * temp = I;
        I = J;
        J = temp;
    }
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
    matrix->root = NULL;
    matrix->is_transposed = 0;
    matrix->k = 0;
    return matrix;
}
void free_matrix_avl(AVLMatrix* matrix){
    _free_o_tree(matrix->root);
    free(matrix);
}
