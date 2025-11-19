#include <stdbool.h>
typedef struct _node * Node; 
typedef struct _hashMatrix * HashMatrix;
struct _node {
    int row, column;
    float data;
    Node next;
};

struct _hashMatrix{
    Node *buckets;
    int capacity, count, rows, columns; //capacity = tamanho total do hash, count = num de elementos não nulos
    bool is_tranposed;
};

HashMatrix createHashMatrix(int rows, int columns);

float getElement(HashMatrix matrix, int row, int column);

void setElement(HashMatrix matrix, int row, int column, float data);

void transpose(HashMatrix matrix);