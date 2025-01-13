#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "minesweeper.h"
#include "probabilities.h"

// Fonction pour initialiser le plateau de jeu
Cell* initBoard(int n, int m){
    // Alloue de la mémoire pour le tableau 3D de cellules
    Cell*** board = malloc(n * sizeof(Cell**));
    for(int i = 0; i < n; i++){
        board[i] = malloc(m * sizeof(Cell*));
        for(int j = 0; j < m; j++){
            // Initialise chaque cellule
            board[i][j] = malloc(sizeof(Cell));
            board[i][j]->x = i;
            board[i][j]->y = j;
            board[i][j]->adjacentMines = 0;
            board[i][j]->isMine = false;
            board[i][j]->isRevealed = false;
            board[i][j]->isFlagged = false;
            board[i][j]->adjacentCells = malloc(8 * sizeof(Cell*));
            board[i][j]->probability = 0.5;
            for(int k = 0; k < 8; k++){
                board[i][j]->adjacentCells[k] = NULL;
            }
        }
    }

    // Établit les liens entre les cellules adjacentes
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            if(i - 1 >= 0 && j - 1 >= 0) board[i][j]->adjacentCells[0] = board[i - 1][j - 1];
            if(i - 1 >= 0) board[i][j]->adjacentCells[1] = board[i - 1][j];
            if(i - 1 >= 0 && j + 1 < m) board[i][j]->adjacentCells[2] = board[i - 1][j + 1];
            if(j + 1 < m) board[i][j]->adjacentCells[4] = board[i][j + 1];
            if(i + 1 < n && j + 1 < m) board[i][j]->adjacentCells[7] = board[i + 1][j + 1];
            if(i + 1 < n) board[i][j]->adjacentCells[6] = board[i + 1][j];
            if(i + 1 < n && j - 1 >= 0) board[i][j]->adjacentCells[5] = board[i + 1][j - 1];
            if(j - 1 >= 0) board[i][j]->adjacentCells[3] = board[i][j - 1];
        }
    }

    // Stocke la cellule d'origine et libère la mémoire du tableau 3D
    Cell* origin = board[0][0];
    for(int i = 0; i < n; i++){free(board[i]);}
    free(board);
    return origin;
}

// Fonction pour ajouter des mines au plateau
void addMines (Cell* origin, int numMines, int n, int m){
    int minesPlaced = 0;
    while(minesPlaced < numMines){
        int x = rand() % m;
        int y = rand() % n;

        Cell* cell = origin;

        // Navigate to the target cell
        for(int j = 0; j < x; j++){
            cell = cell->adjacentCells[4];
        }
        for(int j = 0; j < y; j++){
            cell = cell->adjacentCells[6];
        }

        // Check if the cell already contains a mine
        if(!cell->isMine){
            // Place a mine and increment the adjacent cells' mine counters
            cell->isMine = true;
            minesPlaced++;
          
            for(int j = 0; j < 8; j++){
                if(cell->adjacentCells[j] != NULL){
                    cell->adjacentCells[j]->adjacentMines++;
                }
            }
        }
    }
}

// Fonction pour afficher le plateau
void printBoard(Cell* origin, int n, int m){
    printf("   ");
    for (int i = 0; i < m; i++) {
        printf("%d ", i);  // Affiche les numéros de colonnes
    }
    printf("\n");

    Cell* rowStart = origin;
    for (int i = 0; i < n; i++) {
        printf("%2d ", i);  // Affiche les numéros de lignes
        Cell* cell = rowStart;
        for (int j = 0; j < m; j++) {
            if(cell->isRevealed){
                if(cell->isMine){
                    printf("\033[31m# \033[0m");  // Rouge pour une mine révélée
                } else if(cell->adjacentMines > 0){
                    printf("%d ", cell->adjacentMines);
                } else {
                    printf("\033[0m  ");          // Blanc pour une case vide
                }
            } else if(cell->isFlagged){
                printf("\033[32mD \033[0m");      // Vert pour un drapeau
            } else {
                printf("\033[36m# \033[0m");      // Cyan pour une case non révélée
            }
            if (j < m-1) cell = cell->adjacentCells[4];  // Déplace vers la droite
        }
        printf("\n");
        if (i < n-1) rowStart = rowStart->adjacentCells[6];  // Déplace vers le bas
    }
    printf("\n");
}

void printProbabilityBoard(Cell* origin, int n, int m){
    printf("   ");
    for (int i = 0; i < m; i++) {
        printf("%3d ", i);  // Affiche les numéros de colonnes
    }
    printf("\n");

    Cell* rowStart = origin;
    for (int i = 0; i < n; i++) {
        printf("%3d ", i);  // Affiche les numéros de lignes
        Cell* cell = rowStart;
        for (int j = 0; j < m; j++) {
            if(cell->isRevealed){
                if(cell->isMine){
                    printf("  \033[31m# \033[0m");  // Rouge pour une mine révélée
                } else if(cell->adjacentMines > 0){
                    printf("%3d ", cell->adjacentMines);
                } else {
                    printf("\033[0m    ");          // Blanc pour une case vide
                }
            } else if(cell->isFlagged){
                printf("\033[32mD \033[0m");      // Vert pour un drapeau
            } else {
                printf("%3d ", (int)(cell->probability * 100));
            }
            if (j < m-1) cell = cell->adjacentCells[4];  // Déplace vers la droite
        }
        printf("\n");
        if (i < n-1) rowStart = rowStart->adjacentCells[6];  // Déplace vers le bas
    }
    printf("\n");
}

// Fonction pour libérer la mémoire du plateau
void freeBoard(Cell* board, int size){
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            free(board->adjacentCells[j]);
        }
        free(board);
    }
}

void recursive_reveal(Cell* cell){
    if(cell->isRevealed || cell->isFlagged) return;
    cell->isRevealed = true;
    if(cell->adjacentMines == 0){
        for(int i = 0; i < 8; i++){
            if(cell->adjacentCells[i] != NULL){
                recursive_reveal(cell->adjacentCells[i]);
            }
        }
    }
}

// Fonction principale
int main(){
    int x,y;
    char action;
    int n = 10;
    int m = 10;

    Cell* board = initBoard(n, m);
    printf("Board initialized\n");
    addMines(board, 20, n, m);
    printf("Mines added\n");

    int revealx[] = {3, 2, 2, 2, 3, 3, 4, 4, 4, 0, 5, 5, 5, 6, 7};
    int revealy[] = {4, 2, 7, 8, 7, 8, 7, 8, 9, 9, 2, 8, 9, 7, 3};

    for(int i = 0; i < 15; i++){
        Cell* cell = board;
        for (int j = 0; j < revealx[i]; j++) {
            cell = cell->adjacentCells[6];
        }
        for (int j = 0; j < revealy[i]; j++) {
            cell = cell->adjacentCells[4];
        }
        // recursive_reveal(cell);
    }

    while(1){
        printBoard(board, n, m);
        calculate_probabilities(board, n, m);
        printProbabilityBoard(board, n, m);
        printf("Entrez l'action (r pour révéler, f pour drapeau, q pour quitter): ");
        scanf(" %c", &action);
        if (action == 'q') {
            printf("Merci d'avoir joué !\n");
            break;
        }

        printf("Entrez les coordonnées (x y): ");
        scanf("%d %d", &x, &y);
        if (x < 0 || x >= n || y < 0 || y >= m) {
            printf("Coordonnées invalides.\n");
            continue;
        }

        if (action == 'r'){
            Cell* cell = board;
            for (int i = 0; i < x; i++) {
                cell = cell->adjacentCells[6];
            }
            for (int i = 0; i < y; i++) {
                cell = cell->adjacentCells[4];
            }
            recursive_reveal(cell);
        } else if (action == 'f'){
            Cell* cell = board;
            for (int i = 0; i < x; i++) {
                cell = cell->adjacentCells[6];
            }
            for (int i = 0; i < y; i++) {
                cell = cell->adjacentCells[4];
            }
            cell->isFlagged = !cell->isFlagged;
        } else {
            printf("Action invalide.\n");
        }
    }
    return 0;
}