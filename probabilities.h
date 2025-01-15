#ifndef PROBABILITIES_H
#define PROBABILITIES_H

#include "minesweeper.h"

typedef struct Matrix {
    int rows;
    int cols;
    float** data;
} Matrix;

typedef struct Group {
    int id;
    float* column;
    int cell_count;
    int lower_bound;
    int upper_bound;
    float probability;
    int* cells;
    struct Group* next;
} Group;

typedef struct Solution {
    Matrix* S;
    struct Solution* next;
} Solution;

Matrix* create_matrix(int rows, int cols);
void free_matrix(Matrix* matrix);
void print_matrix(Matrix* matrix);
Matrix* create_adjacency_matrix(int n, int m);
bool float_in_matrix(Matrix* matrix, float value);

Group* create_group(int id, Group* next, float* column, int n, int m);
Group* get_group(Group* groups, int id);
Group* get_group_by_column(Group* groups, float* column, int size);
void print_group(Group* group);

void gauss_jordan(Matrix* matrix);
float dot_product(float* a, float** b, int size);
int factorial(int n);
int nCr(int n, int r);
void calculate_probabilities(Cell* origin, int n, int m);

#endif // PROBABILITIES_H