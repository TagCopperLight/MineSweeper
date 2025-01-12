#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
 
#define MAX_TILES 100
#define MAX_COMBINATIONS 1024
 
// Factorial for combinations count
int factorial(int n) {
    if (n == 0 || n == 1) return 1;
    return n * factorial(n - 1);
}
 
int combinations_count(int n, int k) {
    if (k > n) return 0;
    return factorial(n) / (factorial(k) * factorial(n - k));
}
 
void generate_combinations(int *group, int group_size, int mines, int combinations[][MAX_TILES], int *count) {
    int indices[mines];
    for (int i = 0; i < mines; i++) {
        indices[i] = i;
    }
 
    while (1) {
        int combination[MAX_TILES] = {0};
        for (int i = 0; i < mines; i++) {
            combination[group[indices[i]]] = 1;
        }
        memcpy(combinations[*count], combination, sizeof(combination));
        (*count)++;
 
        int i;
        for (i = mines - 1; i >= 0 && indices[i] == group_size - mines + i; i--);
        if (i < 0) break;
        indices[i]++;
        for (int j = i + 1; j < mines; j++) {
            indices[j] = indices[i] + j - i;
        }
    }
}
 
void calculate_probabilities(int num_tiles, int num_groups, int group_sizes[], int group_tiles[][MAX_TILES], int group_mines[], double probabilities[MAX_TILES]) {
    memset(probabilities, 0, sizeof(double) * num_tiles);
 
    int combinations[num_groups][MAX_COMBINATIONS][MAX_TILES];
    int combination_counts[num_groups];
    memset(combination_counts, 0, sizeof(combination_counts));
 
    for (int g = 0; g < num_groups; g++) {
        generate_combinations(group_tiles[g], group_sizes[g], group_mines[g], combinations[g], &combination_counts[g]);
    }
 
    int total_valid_configurations = 0;
 
    // Iterate over all combinations for all groups
    for (int i = 0; i < combination_counts[0]; i++) {
        bool valid = true;
        int combined[MAX_TILES] = {0};
        for (int t = 0; t < num_tiles; t++) {
            combined[t] = combinations[0][i][t];
        }
 
        for (int g = 1; g < num_groups; g++) {
            bool group_valid = false;
            for (int j = 0; j < combination_counts[g]; j++) {
                bool overlap_valid = true;
                for (int t = 0; t < num_tiles; t++) {
                    if (combined[t] && combinations[g][j][t]) {
                        overlap_valid = false;
                        break;
                    }
                }
                if (overlap_valid) {
                    group_valid = true;
                    for (int t = 0; t < num_tiles; t++) {
                        combined[t] |= combinations[g][j][t];
                    }
                    break;
                }
            }
            if (!group_valid) {
                valid = false;
                break;
            }
        }
 
        if (!valid) continue;
 
        total_valid_configurations++;
        for (int t = 0; t < num_tiles; t++) {
            probabilities[t] += combined[t];
        }
    }
 
    for (int t = 0; t < num_tiles; t++) {
        probabilities[t] /= total_valid_configurations;
    }
}
 
int main() {
    // Define the board and groups
    int num_tiles = 10; // Total tiles on the board
 
    int num_groups = 2; // Number of groups (constraints)
    int group_sizes[] = {7, 7}; // Sizes of the groups
    int group_tiles[2][MAX_TILES] = {
        {0, 1, 2, 3, 4, 5, 6}, // Group affected by '2'
        {3, 4, 5, 6, 7, 8, 9}  // Group affected by '1'
    };
    int group_mines[] = {2, 1}; // Mines in each group
 
    double probabilities[MAX_TILES];
    calculate_probabilities(num_tiles, num_groups, group_sizes, group_tiles, group_mines, probabilities);
 
    printf("Probabilities of a mine being present:\n");
    for (int i = 0; i < num_tiles; i++) {
        printf("Tile %c: %.2f\n", 'A' + i, probabilities[i]);
    }
 
    return 0;
}