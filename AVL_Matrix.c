#include "AVL_Matrix.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @file AVL_Matrix.c
 * @brief Implementação de matriz esparsa com duas árvores AVL (linhas e colunas).
 *
 * O arquivo contém funções internas de balanceamento e validação, além das
 * operações públicas declaradas em AVL_Matrix.h.
 */

/**
 * @brief Encerramento imediato em caso de falha de alocação.
 *
 * Imprime uma mensagem de erro em stderr e aborta o processo usando EXIT_FAILURE.
 */
static void _allocation_fail(){
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

/**
 * @brief Valida ponteiro da matriz e dimensões armazenadas.
 *
 * @param matrix ponteiro para a matriz a ser checada.
 * @return Código ::AVLStatus indicando sucesso ou motivo da falha.
 */
static AVLStatus _validate_matrix(AVLMatrix* matrix){
    if(!matrix){
        return AVL_ERROR_NULL_MATRIX;
    }
    if(matrix->n < 0 || matrix->m < 0){
        return AVL_ERROR_INVALID_ARGUMENT;
    }
    return AVL_STATUS_OK;
}

/**
 * @brief Verifica se índices i,j estão dentro dos limites da matriz.
 *
 * @param matrix matriz onde a posição será utilizada.
 * @param i índice de linha.
 * @param j índice de coluna.
 * @return Código ::AVLStatus indicando sucesso ou motivo da falha.
 */
static AVLStatus _validate_indexes(AVLMatrix* matrix, int i, int j){
    AVLStatus status = _validate_matrix(matrix);
    if(status != AVL_STATUS_OK){
        return status;
    }
    if(i < 0 || i >= matrix->n || j < 0 || j >= matrix->m){
        return AVL_ERROR_OUT_OF_BOUNDS;
    }
    return AVL_STATUS_OK;
}

/**
 * @brief Garante que duas matrizes têm dimensões compatíveis (mesma ordem).
 *
 * @param A primeira matriz.
 * @param B segunda matriz.
 * @return Código ::AVLStatus indicando sucesso ou motivo da falha.
 */
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

/**
 * @brief Retorna altura de um nó da árvore interna (0 se nulo).
 *
 * @param tree nó cuja altura é consultada.
 */
static int _height_i(InnerNode* tree){
    if(!tree){
        return 0; 
    }
    return tree->height;
}
/**
 * @brief Retorna altura de um nó da árvore externa (0 se nulo).
 *
 * @param tree nó cuja altura é consultada.
 */
static int _height_o(OuterNode* tree){
    if(!tree){
        return 0; 
    }
    return tree->height;
}
/**
 * @brief Calcula fator de balanceamento (altura esquerda - altura direita) da árvore interna.
 *
 * @param tree raiz da subárvore interna.
 */
static int _balance_factor_i(InnerNode* tree){
    return _height_i(tree->left) - _height_i(tree->right);
}
/**
 * @brief Calcula fator de balanceamento (altura esquerda - altura direita) da árvore externa.
 *
 * @param tree raiz da subárvore externa.
 */
static int _balance_factor_o(OuterNode* tree){
    return _height_o(tree->left) - _height_o(tree->right);
}

/**
 * @brief Retorna o maior entre dois inteiros.
 *
 * @param a primeiro inteiro.
 * @param b segundo inteiro.
 */
static int _max(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * @brief Rotação à esquerda na árvore interna.
 *
 * Rotação usada para o rebalanceamento da árvore
 * AVL interna.
 *
 * @param tree raiz desbalanceada.
 * @return Nova raiz após rotação.
 */
static InnerNode* _left_rotate_i(InnerNode* tree){
    InnerNode* right = tree->right;
    tree -> right = right->left;
    right->left = tree;

    tree->height  = 1 + _max(_height_i(tree->left),  _height_i(tree->right));
    right->height = 1 + _max(_height_i(right->left), _height_i(right->right));
    return right;
}

/**
 * @brief Rotação à direita na árvore interna.
 *
 * Rotação usada para o rebalanceamento da árvore
 * AVL interna.
 *
 * @param tree raiz desbalanceada.
 * @return Nova raiz após rotação.
 */
static InnerNode* _right_rotate_i(InnerNode* tree){
    InnerNode* left = tree->left;
    tree -> left = left-> right;
    left->right = tree;

    tree->height = 1 + _max(_height_i(tree->left), _height_i(tree->right));
    left->height = 1 + _max(_height_i(left->left), _height_i(left->right));
    return left;
}

/**
 * @brief Rotação à esquerda na árvore externa.
 * 
 * Rotação usada para o rebalanceamento da árvore
 * AVL externa.
 *
 * @param tree raiz desbalanceada.
 * @return Nova raiz após rotação.
 */
static OuterNode* _left_rotate_o(OuterNode* tree){
    OuterNode* right = tree->right;
    tree->right = right->left;
    right->left = tree;

    tree->height  = 1 + _max(_height_o(tree->left),  _height_o(tree->right));
    right->height = 1 + _max(_height_o(right->left), _height_o(right->right));

    return right;
}

/**
 * @brief Rotação à direita na árvore externa.
 * 
 * Rotação usada para o rebalanceamento da árvore
 * AVL externa.
 *
 * @param tree raiz desbalanceada.
 * @return Nova raiz após rotação.
 */
static OuterNode* _right_rotate_o(OuterNode* tree){
    OuterNode* left = tree->left;
    tree->left = left->right;
    left->right = tree;

    tree->height = 1 + _max(_height_o(tree->left), _height_o(tree->right));
    left->height = 1 + _max(_height_o(left->left), _height_o(left->right));

    return left;
}

/**
 * @brief Busca nó na árvore interna pelo índice.
 *
 * @param tree raiz da árvore interna.
 * @param search_key fileira procurada.
 * @return Ponteiro para o nó ou NULL se não existir.
 */
static InnerNode* _find_node_i(InnerNode* tree, int search_key){
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

/**
 * @brief Busca nó na árvore externa pelo índice.
 *
 * @param tree raiz da árvore externa.
 * @param search_key fileira procurada.
 * @return Ponteiro para o nó ou NULL se não existir.
 */
static OuterNode* _find_node_o(OuterNode* tree, int search_key){
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

/**
 * @brief Insere ou atualiza um valor na árvore interna mantendo balanceamento AVL.
 *
 * @param tree raiz da árvore interna.
 * @param insert_key índice do elemento a inserir.
 * @param value valor a armazenar.
 * @param already_existed flag de saída: 1 se chave já existia.
 * @return Nova raiz da subárvore após inserção/balanceamento.
 */
static InnerNode* _insert_i(InnerNode* tree, int insert_key, float value, int* already_existed){
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

/**
 * @brief Insere ou atualiza um valor na árvore externa mantendo balanceamento AVL.
 *
 * @param tree raiz da árvore externa.
 * @param insert_key fileira a inserir.
 * @param inner_tree ponteiro para a árvore interna à ser inserida no nó.
 * @return Nova raiz da subárvore após inserção/balanceamento.
 */
static OuterNode* _insert_o(OuterNode* tree, int insert_key, InnerNode* inner_tree){
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
/**
 * @brief Encontra o maior nó (mais à direita) em uma árvore interna.
 *
 * @param tree raiz da árvore interna.
 * @return Ponteiro para o nó máximo ou NULL se a árvore estiver vazia.
 */
static InnerNode * _find_max_i (InnerNode * tree){
    if(tree == NULL){
        return NULL;
    }
    if(tree -> right == NULL){
        return tree;
    }
    return _find_max_i(tree -> right);
}

/**
 * @brief Encontra o maior nó (mais à direita) em uma árvore externa.
 *
 * @param tree raiz da árvore externa.
 * @return Ponteiro para o nó máximo ou NULL se a árvore estiver vazia.
 */
static OuterNode * _find_max_o (OuterNode * tree){
    if(tree == NULL){
        return NULL;
    }
    if(tree -> right == NULL){
        return tree;
    }
    return _find_max_o(tree -> right);
}

/**
 * @brief Remove chave da árvore interna e rebalanceia.
 *
 * @param tree raiz da árvore interna.
 * @param remove_key coluna a remover.
 * @return Nova raiz da subárvore após remoção.
 */
static InnerNode * _remove_i(InnerNode* tree, int remove_key){
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

/**
 * @brief Remove fileira e rebalanceia a árvore externa.
 * 
 * Só deve ser chamado para a remoção "estrutural" do nó,
 * não limpa o conteúdo interno dele. Planejado para a
 * remoção de um nó já vazio.
 *
 * @param tree raiz da árvore externa.
 * @param remove_key fileira a remover.
 * @return Nova raiz da subárvore após remoção.
 */
static OuterNode * _remove_o(OuterNode* tree, int remove_key){
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

/**
 * @brief Libera recursivamente uma árvore interna.
 *
 * @param tree raiz da árvore interna a ser desalocada (ou NULL).
 */
static void _free_i_tree(InnerNode* tree){
    if(tree == NULL){
        return;
    }
    _free_i_tree(tree->left);
    _free_i_tree(tree->right);
    free(tree);
    return;
}

/**
 * @brief Libera recursivamente árvore externa e todas as internas associadas.
 *
 * @param tree raiz da árvore externa a ser desalocada (ou NULL).
 */
static void _free_o_tree(OuterNode* tree){
    if(tree == NULL){
        return;
    }
    _free_o_tree(tree->left);
    _free_o_tree(tree->right);
    _free_i_tree(tree->inner_tree);
    free(tree);
    return;
}

/**
 * @brief Clona profundamente uma árvore interna.
 *
 * @param tree raiz a copiar.
 * @return Ponteiro para a nova raiz clonada.
 */
static InnerNode* _clone_i_tree(InnerNode* tree){
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
    new_node->left = _clone_i_tree(tree->left);
    new_node->right = _clone_i_tree(tree->right);
    return new_node;
}

/**
 * @brief Clona profundamente a árvore externa e cada árvore interna.
 *
 * @param tree raiz a copiar.
 * @return Ponteiro para a nova raiz clonada.
 */
static OuterNode* _clone_o_tree(OuterNode* tree){
    if(!tree){
        return NULL;
    }
    OuterNode* new_node = malloc(sizeof(OuterNode));
    if(!new_node){
        _allocation_fail();
    }
    new_node->key = tree->key;
    new_node->height = tree->height;
    new_node->inner_tree = _clone_i_tree(tree->inner_tree);
    new_node->left = _clone_o_tree(tree->left);
    new_node->right = _clone_o_tree(tree->right);
    return new_node;
}

/**
 * @brief Copia conteúdo de uma matriz para outra, recriando ambas as árvores.
 *
 * @param source origem (já validada).
 * @param dest destino, que terá árvores antigas liberadas.
 */
static AVLStatus _copy_matrix(AVLMatrix* source, AVLMatrix* dest){
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
    dest->main_root = _clone_o_tree(source->main_root);
    dest->transposed_root = _clone_o_tree(source->transposed_root);
    dest->k = source->k;
    dest->n = source->n;
    dest->m = source->m;
    return AVL_STATUS_OK;
}

/**
 * @brief Multiplica todos os nós de uma árvore interna por um escalar.
 *
 * @param tree raiz da árvore interna.
 * @param a fator escalar.
 */
static void _scalar_multiply_i_tree(InnerNode* tree, float a){
    if(!tree){
        return;
    }
    _scalar_multiply_i_tree(tree->left, a);
    tree->data = tree->data * a;
    _scalar_multiply_i_tree(tree->right, a);
}

/**
 * @brief Multiplica todos os valores armazenados na árvore externa (todas as fileiras) por um escalar.
 *
 * @param tree raiz da árvore externa.
 * @param a fator escalar.
 */
static void _scalar_multiply_o_tree(OuterNode* tree, float a){
    if(!tree){
        return;
    }
    _scalar_multiply_o_tree(tree->left, a);
    _scalar_multiply_i_tree(tree->inner_tree, a);
    _scalar_multiply_o_tree(tree->right, a);
}

/**
 * @brief Copia pares (i,j,valor) de uma árvore interna para vetores auxiliares.
 *
 * @param inner_tree árvore interna.
 * @param I vetor de linhas.
 * @param J vetor de colunas.
 * @param Data vetor de valores.
 * @param position ponteiro com posição inicial dos vetores.
 * @param i índice de linha fixado.
 */
static void _copy_i(InnerNode* inner_tree, int* I, int* J, float* Data, int* position, int i){
    if(!inner_tree){
        return;
    }

    _copy_i(inner_tree -> left, I, J, Data, position, i);
    I[*position] = i;
    J[*position] = inner_tree->key;
    Data[*position] = inner_tree->data;
    *position = *position + 1;
    _copy_i(inner_tree -> right, I, J, Data, position, i);
    return;
}

/**
 * @brief Copia toda a matriz percorrendo a árvore externa e delegando para _copy_i.
 *
 * Resulta em vetores paralelos I,J,Data de tamanho k com todas as entradas não nulas.
 *
 * @param outer_tree raiz da árvore externa.
 * @param I vetor de linhas.
 * @param J vetor de colunas.
 * @param Data vetor de valores.
 * @param position ponteiro com posição inicial dos vetores.
 */
static void _copy_o(OuterNode* outer_tree, int* I, int* J, float* Data, int* position){
    if(!outer_tree){
        return;
    }

    _copy_o(outer_tree -> left, I, J, Data, position);
    _copy_i(outer_tree->inner_tree, I, J, Data, position, outer_tree->key);
    _copy_o(outer_tree-> right, I, J, Data, position);
    return;
}

/**
 * @brief Acumula parcial de multiplicação de matrizes: atualiza linha de C com valores de A * B.
 *
 * Percorre a árvore interna correspondente à linha j de B (elementos B[j, c])
 * e, para cada coluna c ali presente, soma no resultado C a contribuição
 * A[i, j] * B[j, c]. O efeito é acumular na linha i de C todas as parcelas
 * referentes a um valor específico A[i, j], de modo que, com chamadas que
 * englobam todas as posições necessárias, a multiplicação de matriz ocorre
 * corretamente.
 *
 * @param inner_tree árvore interna da linha j de B (colunas não nulas).
 * @param row linha alvo em C.
 * @param A_value valor A[i, j] correspondente.
 * @param C matriz resultado.
 */
static AVLStatus _matmul_i_accumulate(InnerNode* inner_tree, int row, float A_value, AVLMatrix* C){
    if(!inner_tree){
        return AVL_STATUS_OK;
    }
    AVLStatus status = _matmul_i_accumulate(inner_tree->left, row, A_value, C);
    if(status != AVL_STATUS_OK){
        return status;
    }
    float current = 0.0f;
    status = get_element_avl(C, row, inner_tree->key, &current);
    if(status != AVL_STATUS_OK){
        return status;
    }
    status = insert_element_avl(C, current + (A_value * inner_tree->data), row, inner_tree->key);
    if(status != AVL_STATUS_OK){
        return status;
    }
    return _matmul_i_accumulate(inner_tree->right, row, A_value, C);
}

AVLStatus get_element_avl(AVLMatrix* matrix, int i, int j, float* out_value){
    if(!out_value){
        return AVL_ERROR_INVALID_ARGUMENT;
    }
    AVLStatus status = _validate_matrix(matrix);
    if(status != AVL_STATUS_OK){
        return status;
    }
    *out_value = 0.0f;
    status = _validate_indices(matrix, i, j);
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
        _scalar_multiply_o_tree(A->main_root, a);
        _scalar_multiply_o_tree(A->transposed_root, a);
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
    _scalar_multiply_o_tree(B->main_root, a);
    _scalar_multiply_o_tree(B->transposed_root, a);
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
    _copy_o(A->main_root, I, J, Data, &position);
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
    if(!(A-> m == B -> n)){
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
    if(A ->k == 0 || B->k == 0){
        return AVL_STATUS_OK;
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
    _copy_o(A->main_root, I, J, Data, &position);
    for(int pos = 0; pos < A->k; pos++){
        OuterNode* B_row = _find_node_o(B->main_root, J[pos]);
        if(B_row){
            AVLStatus status = _matmul_i_accumulate(B_row->inner_tree, I[pos], Data[pos], C);
            if(status != AVL_STATUS_OK){
                free(I);
                free(J);
                free(Data);
                return status;
            }
        }
    }

    free(I);
    free(J);
    free(Data);
    
    return AVL_STATUS_OK;
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
