#ifndef PROBABILITIES_H
#define PROBABILITIES_H

#include "minesweeper.h"

typedef struct Matrix {
    int rows;
    int cols;
    float** data;
} Matrix;

Matrix* create_matrix(int rows, int cols);
void free_matrix(Matrix* matrix);
void print_matrix(Matrix* matrix);

void calculate_probabilities(Cell* origin, int size);

#endif // PROBABILITIES_H