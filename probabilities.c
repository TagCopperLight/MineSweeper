#include <stdio.h>
#include <stdlib.h>
#include "probabilities.h"
#include "minesweeper.h"


Matrix* create_matrix(int rows, int cols){
    Matrix* matrix = malloc(sizeof(Matrix));
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = malloc(rows * sizeof(float*));
    for (int i = 0; i < rows; i++){
        matrix->data[i] = malloc(cols * sizeof(float));
        for (int j = 0; j < cols; j++){
            matrix->data[i][j] = 0;
        }
    }
    return matrix;	
}

void free_matrix(Matrix* matrix){
    for (int i = 0; i < matrix->rows; i++){
        free(matrix->data[i]);
    }
    free(matrix->data);
    free(matrix);
}

void print_matrix(Matrix* matrix){
    for (int i = 0; i < matrix->rows; i++){
        for (int j = 0; j < matrix->cols; j++){
            printf("%i ", (int)matrix->data[i][j]);
        }
        printf("\n");
    }
}

Matrix* create_adjacency_matrix(int n, int m){
    Matrix* A = create_matrix(n*m, n*m);

    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            int index = i*m + j;
            if (i - 1 >= 0 && j - 1 >= 0) A->data[index][index - m - 1] = 1;
            if (j - 1 >= 0) A->data[index][index - 1] = 1;
            if (i + 1 < n && j - 1 >= 0) A->data[index][index + m - 1] = 1;
            if (i - 1 >= 0) A->data[index][index - m] = 1;
            if (i + 1 < n) A->data[index][index + m] = 1;
            if (i - 1 >= 0 && j + 1 < m) A->data[index][index - m + 1] = 1;
            if (j + 1 < m) A->data[index][index + 1] = 1;
            if (i + 1 < n && j + 1 < m) A->data[index][index + m + 1] = 1;
        }
    }

    return A;
}

void calculate_probabilities(Cell* origin, int size){
    int n = 4;
    int m = 3;
    Matrix* A = create_adjacency_matrix(n, m);
    print_matrix(A);

    Matrix* v = create_matrix(n*m, 1);
    for (int i = 0; i < n; i++){
        Cell* cell = origin;
        for (int j = 0; j < i; j++){
            cell = cell->adjacentCells[6];
        }
        for (int j = 0; j < m; j++){
            if (cell->isRevealed){
                v->data[i*m + j][0] = cell->adjacentMines;
            } else {
                v->data[i*m + j][0] = -1;
            }
            cell = cell->adjacentCells[4];
        }
    }

    print_matrix(v);
}