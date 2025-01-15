#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
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
    printf("   ");
    for (int i = 0; i < matrix->cols; i++){
        printf("\033[36m%2d \033[0m", i);
    }
    printf("\n");
    for (int i = 0; i < matrix->rows; i++){
        printf("\033[36m%2d \033[0m", i);
        for (int j = 0; j < matrix->cols; j++){
            printf("%2d ", (int)matrix->data[i][j]);
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

bool float_in_matrix(Matrix* matrix, float value){
    for (int i = 0; i < matrix->rows; i++){
        for (int j = 0; j < matrix->cols; j++){
            if (matrix->data[i][j] == value){
                return true;
            }
        }
    }
    return false;
}

Group* create_group(int id, Group* next, float* column, int n, int m){
    Group* group = malloc(sizeof(Group));
    group->id = id;
    group->column = column;
    group->cell_count = 0;
    group->lower_bound = 0;
    group->upper_bound = 0;
    group->probability = 0;
    group->cells = malloc(n*m * sizeof(int));
    group->next = next;
    return group;
}

Group* get_group(Group* groups, int id){
    Group* group = groups;
    while (group != NULL){
        if (group->id == id){
            return group;
        }
        group = group->next;
    }
    return NULL;
}

Group* get_group_by_column(Group* groups, float* column, int size){
    Group* group = groups;
    while (group != NULL){
        bool is_same = true;
        for (int i = 0; i < size; i++){
            if (group->column[i] != column[i]){
                is_same = false;
                break;
            }
        }
        if (is_same){
            return group;
        }
        group = group->next;
    }
    return NULL;
}

void print_group(Group* group){
    printf("Group %d:\n", group->id);
    printf("Cell count: %d\n", group->cell_count);
    printf("Lower bound: %d\n", group->lower_bound);
    printf("Upper bound: %d\n", group->upper_bound);
    printf("Probability: %f\n", group->probability);
    printf("Cells: ");
    for (int i = 0; i < group->cell_count; i++){
        printf("%d ", group->cells[i]);
    }
    printf("\n");
    if (group->next != NULL){
        printf("\n");
        print_group(group->next);
    }
}

void gauss_jordan(Matrix* matrix){
    int rows = matrix->rows;
    int cols = matrix->cols;

    int r = 0;

    for (int j = 0; j < cols; j++){
        if (r >= rows){
            break;
        }

        int k = r;
        int k_value = fabs(matrix->data[k][j]);
        for (int i = r + 1; i < rows; i++){
            if (fabs(matrix->data[i][j]) > k_value){
                k = i;
                k_value = fabs(matrix->data[i][j]);
            }
        }

        if (matrix->data[k][j] != 0){
            if (k != r){
                for (int i = 0; i < cols; i++){
                    float temp = matrix->data[r][i];
                    matrix->data[r][i] = matrix->data[k][i];
                    matrix->data[k][i] = temp;
                }
            }

            float pivot = matrix->data[r][j];
            for (int i = 0; i < cols; i++){
                matrix->data[r][i] /= pivot;
            }

            for (int i = 0; i < rows; i++){
                if (i != r){
                    float factor = matrix->data[i][j];
                    for (int l = 0; l < cols; l++){
                        matrix->data[i][l] -= factor * matrix->data[r][l];
                    }
                }
            }

            r++;
        }
    }
}

float dot_product(float* a, float** b, int size){
    float result = 0;
    for (int i = 0; i < size; i++){
        result += a[i] * b[i][0];
    }
    return result;
}

int factorial(int n){
    if (n < 0) return 0;
    if (n <= 1) return 1;

    int result = 1;
    int i = 2;

    do {
        result *= i++;
    } while (i <= n);

    return result;
}

int nCr(int n, int r){
    return factorial(n) / (factorial(r) * factorial(n - r));
}

void calculate_probabilities(Cell* origin, int n, int m){
    Matrix* A = create_adjacency_matrix(n, m);
    // printf("A:\n");
    // print_matrix(A);

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

    // printf("\nv:\n");
    // print_matrix(v);

    int n_of_reveald_cells = 0;
    for (int i = 0; i < n*m; i++){
        if (v->data[i][0] != -1){
            n_of_reveald_cells++;
        }
    }

    // printf("\nn_of_reveald_cells: %d\n", n_of_reveald_cells);

    if (n_of_reveald_cells == 0 || n_of_reveald_cells == n*m){
        return;
    }

    Matrix* revealed_cells = create_matrix(1, n_of_reveald_cells);

    int k = 0;
    for (int i = 0; i < n*m; i++){
        if (v->data[i][0] != -1){
            revealed_cells->data[0][k] = i;
            k++;
        }
    }

    // printf("\nrevealed_cells:\n");
    // print_matrix(revealed_cells);
    // A_prime represents the matrix A with only the lines of the revealed cells
    Matrix* A_prime = create_matrix(n_of_reveald_cells, n*m);
    for (int i = 0; i < n_of_reveald_cells; i++){
        for (int j = 0; j < n*m; j++){
            A_prime->data[i][j] = A->data[(int)revealed_cells->data[0][i]][j];
        }
    }

    // printf("\nA':\n");
    // print_matrix(A_prime);

    Matrix* v_prime = create_matrix(n_of_reveald_cells, 1);
    for (int i = 0; i < n_of_reveald_cells; i++){
        v_prime->data[i][0] = v->data[(int)revealed_cells->data[0][i]][0];
    }

    // printf("\nv':\n");
    // print_matrix(v_prime);

    int n_of_null_columns = 0;
    for (int i = 0; i < n*m; i++){
        if (float_in_matrix(revealed_cells, i)){
            continue;
        }
        int sum = 0;
        for (int j = 0; j < n_of_reveald_cells; j++){
            sum += A_prime->data[j][i];
        }
        if (sum == 0){
            n_of_null_columns++;
        }
    }

    // printf("\nn_of_null_columns: %d\n", n_of_null_columns);

    int m_prime = n*m - n_of_null_columns - n_of_reveald_cells;
    // printf("\nm_prime: %d\n", m_prime);

    Matrix* A_prime_filtered = create_matrix(n_of_reveald_cells, m_prime);
    k = 0;
    for (int i = 0; i < n*m ; i++){
        if (float_in_matrix(revealed_cells, i)){
            continue;
        }
        int sum = 0;
        for (int j = 0; j < n_of_reveald_cells; j++){
            sum += A_prime->data[j][i];
        }
        if (sum == 0){
            continue;
        }
        for (int j = 0; j < n_of_reveald_cells; j++){
            A_prime_filtered->data[j][k] = A_prime->data[j][i];
        }
        k++;
    }

    // printf("\nA'_filtered:\n");
    // print_matrix(A_prime_filtered);

    Group* groups = NULL;
    int n_of_groups = 0;

    for (int i = 0; i < n*m; i++){
        if (float_in_matrix(revealed_cells, i)){
            continue;
        }
        int sum = 0;
        for (int j = 0; j < n_of_reveald_cells; j++){
            sum += A_prime->data[j][i];
        }
        if (sum == 0){
            continue;
        }
        float* column = malloc(n_of_reveald_cells * sizeof(float));
        
        for (int j = 0; j < n_of_reveald_cells; j++){
            column[j] = A_prime->data[j][i];
        }

        Group* group = get_group_by_column(groups, column, n_of_reveald_cells);
        if (group == NULL){
            group = create_group(n_of_groups, groups, column, n, m);
            groups = group;
            n_of_groups++;
        }

        group->cells[group->cell_count] = i;
        group->cell_count++;
        group->upper_bound++;
    }

    // printf("\nGroups:\n");
    // print_group(groups);
    // printf("\nn_of_groups: %d\n", n_of_groups);

    Matrix* A_second = create_matrix(n_of_reveald_cells, n_of_groups);
    k = n_of_groups - 1;
    Group* group = groups;
    while (group != NULL){
        for (int i = 0; i < n_of_reveald_cells; i++){
            A_second->data[i][k] = A_prime->data[i][group->cells[0]];
        }
        k--;
        group = group->next;
    }

    // printf("\nA_second:\n");
    // print_matrix(A_second);

    Matrix* system = create_matrix(n_of_reveald_cells, n_of_groups + 1);
    for (int i = 0; i < n_of_reveald_cells; i++){
        for (int j = 0; j < n_of_groups; j++){
            system->data[i][j] = A_second->data[i][j];
        }
        system->data[i][n_of_groups] = v_prime->data[i][0];
    }

    // printf("\nSystem:\n");
    // print_matrix(system);

    // printf("\nGroup Ids:\n");
    // for (int i = 0; i < n_of_groups; i++){
    //     printf("%d ", i);
    // }
    // printf("\n");

    for (int i = 0; i < n_of_reveald_cells; i++){
        float* equation = system->data[i];
        for (int j = 0; j < n_of_groups - 1; j++){
            if (equation[j] == 1){
                Group* group = get_group(groups, j);
                if (equation[n_of_groups] < group->lower_bound){
                    group->upper_bound = equation[n_of_groups];
                }
            }
        }
    }

    // printf("\nGroups after bonds:\n");
    // print_group(groups);

    gauss_jordan(system);

    // printf("\nSystem after gauss-jordan:\n");
    // print_matrix(system);

    Solution* solutions = NULL;
    int n_of_solutions = 0;

    int* pivot_columns = malloc(n_of_groups * sizeof(int));
    int n_of_pivot_columns = 0;
    for (int i = 0; i < n_of_reveald_cells; i++){
        for (int j = 0; j < n_of_groups; j++){
            if (system->data[i][j] == 1){
                bool all_zeros = true;
                for (int k = 0; k < n_of_reveald_cells; k++){
                    if (k != i && system->data[k][j] != 0){
                        all_zeros = false;
                        break;
                    }
                }
                if (all_zeros){
                    pivot_columns[n_of_pivot_columns] = j;
                    n_of_pivot_columns++;
                }
            }
        }
    }

    // printf("\nPivot columns:\n");
    // for (int i = 0; i < n_of_pivot_columns; i++){
    //     printf("%d ", pivot_columns[i]);
    // }
    // printf("\nn_of_pivot_columns: %d\n", n_of_pivot_columns);

    int* free_columns = malloc(n_of_groups * sizeof(int));
    int n_of_free_columns = 0;
    for (int i = 0; i < n_of_groups; i++){
        bool is_pivot = false;
        for (int j = 0; j < n_of_pivot_columns; j++){
            if (i == pivot_columns[j]){
                is_pivot = true;
                break;
            }
        }
        if (!is_pivot){
            free_columns[n_of_free_columns] = i;
            n_of_free_columns++;
        }
    }

    // printf("\nFree columns:\n");
    // for (int i = 0; i < n_of_free_columns; i++){
    //     printf("%d ", free_columns[i]);
    // }
    // printf("\nn_of_free_columns: %d\n", n_of_free_columns);

    int** free_columns_ranges = malloc(n_of_free_columns * sizeof(int*));
    int* free_columns_ranges_sizes = malloc(n_of_free_columns * sizeof(int));
    for (int i = 0; i < n_of_free_columns; i++){
        Group* group = get_group(groups, free_columns[i]);

        int lower_bound = group->lower_bound;
        int upper_bound = group->upper_bound;

        int range_size = upper_bound - lower_bound + 1;
        free_columns_ranges[i] = malloc(range_size * sizeof(int));
        free_columns_ranges_sizes[i] = range_size;

        for (int j = 0; j < range_size; j++){
            free_columns_ranges[i][j] = lower_bound + j;
        }
    }

    // printf("\nFree columns ranges:\n");
    // for (int i = 0; i < n_of_free_columns; i++){
    //     printf("Group %d: ", free_columns[i]);
    //     for (int j = 0; j < free_columns_ranges_sizes[i]; j++){
    //         printf("%d ", free_columns_ranges[i][j]);
    //     }
    // }
    // printf("\n\n");

    int* current_indices = calloc(n_of_free_columns, sizeof(int));

    while (1){
        Matrix* solution = create_matrix(n_of_groups, 1);

        for (int i = 0; i < n_of_free_columns; i++){
            solution->data[free_columns[i]][0] = free_columns_ranges[i][current_indices[i]];
        }

        for (int i = 0; i < n_of_pivot_columns; i++){
            solution->data[pivot_columns[i]][0] = system->data[i][n_of_groups] - dot_product(system->data[i], solution->data, n_of_groups);
        }

        bool is_valid = true;
        for (int i = 0; i < n_of_groups; i++){
            Group* group = get_group(groups, i);

            if (solution->data[i][0] < group->lower_bound || solution->data[i][0] > group->upper_bound){
                is_valid = false;
                break;
            }
        }

        if (is_valid){
            Solution* new_solution = malloc(sizeof(Solution));
            new_solution->S = solution;
            new_solution->next = solutions;
            solutions = new_solution;
            n_of_solutions++;
        } else {
            free_matrix(solution);
        }

        int pos = n_of_free_columns - 1;
        while (pos >= 0){
            current_indices[pos]++;
            if (current_indices[pos] == free_columns_ranges_sizes[pos]){
                current_indices[pos] = 0;
                pos--;
            } else {
                break;
            }
        }
        if (pos < 0){
            break;
        }
    }

    // printf("\nSolutions:\n");
    // Solution* current_solution = solutions;
    // while (current_solution != NULL){
    //     print_matrix(current_solution->S);
    //     printf("\n");
    //     current_solution = current_solution->next;
    // }

    float* solutions_probabilities = malloc(n_of_solutions * sizeof(float));
    Solution* current_solution = solutions;

    k = 0;
    while (current_solution != NULL){
        float probability = 1;
        for (int i = 0; i < n_of_groups; i++){
            Group* group = get_group(groups, i);
            probability *= nCr(group->cell_count, (int)current_solution->S->data[i][0]);
        }
        solutions_probabilities[k] = probability;
        k++;
        current_solution = current_solution->next;
    }
    int sum = 0;
    for (int i = 0; i < n_of_solutions; i++){
        sum += solutions_probabilities[i];
    }
    for (int i = 0; i < n_of_solutions; i++){
        solutions_probabilities[i] /= sum;
    }

    // printf("\nProbabilities:\n");
    // current_solution = solutions;
    // for (int i = 0; i < n_of_solutions; i++){
    //     print_matrix(current_solution->S);
    //     printf("Probability: %f\n", solutions_probabilities[i]);
    //     printf("\n");
    //     current_solution = current_solution->next;
    // }

    Group* current_group = groups;
    while (current_group != NULL){
        float group_probability = 0;
        current_solution = solutions;
        int i = 0;
        while (current_solution != NULL){
            for (int j = 0; j < n_of_groups; j++){
                Group* group = get_group(groups, j);
                if (group->id == current_group->id){
                    group_probability += solutions_probabilities[i] * current_solution->S->data[j][0] / group->cell_count;
                }
            }
            current_solution = current_solution->next;
            i++;
        }
        current_group->probability = group_probability;
        current_group = current_group->next;
    }

    // printf("\nGroups with probabilities:\n");
    // print_group(groups);

    for (int i = 0; i < n; i++){
        Cell* cell = origin;
        for (int j = 0; j < i; j++){
            cell = cell->adjacentCells[6];
        }
        for (int j = 0; j < m; j++){
            for (int k = 0; k < n_of_groups; k++){
                Group* group = get_group(groups, k);
                for (int l = 0; l < group->cell_count; l++){
                    if (group->cells[l] == i*m + j){
                        cell->probability = group->probability;
                        break;
                    }
                }                
            }
            cell = cell->adjacentCells[4];
        }
    }
}