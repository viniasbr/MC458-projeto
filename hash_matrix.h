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
    int capacity, count, rows, columns;
    bool is_tranposed;
};


HashMatrix createHashMatrix(int rows, int columns, int capacity);

float getElement(HashMatrix matrix, int row, int column);

void setElement(HashMatrix matrix, int row, int column);

void transpose(HashMatrix matrix);