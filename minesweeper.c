#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
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
void addMines (Cell* origin, int numMines, int n, int m, int init_x, int init_y){
    if (numMines > n * m){
        printf("Trop de mines pour le plateau\n");
        exit(1);
    }
    int minesPlaced = 0;
    while(minesPlaced < numMines){
        int x = rand() % m;
        int y = rand() % n;

        if(init_x - 1 <= x && x <= init_x + 1 && init_y - 1 <= y && y <= init_y + 1){
            continue;
        }

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
        printf("%2d ", i);  // Affiche les numéros de colonnes
    }
    printf("\n");

    Cell* rowStart = origin;
    for (int i = 0; i < n; i++) {
        printf("%2d  ", i);  // Affiche les numéros de lignes
        Cell* cell = rowStart;
        for (int j = 0; j < m; j++) {
            if(cell->isRevealed){
                if(cell->isMine){
                    if(m < 9) printf("\033[31m#  \033[0m");  // Rouge pour une mine révélée
                    else printf("\033[31m#  \033[0m");
                } else if(cell->adjacentMines > 0){
                    if(m < 0) printf("%d  ", cell->adjacentMines);
                    else printf("%d  ", cell->adjacentMines);
                } else {
                    printf("\033[0m   ");          // Blanc pour une case vide
                }
            } else if(cell->isFlagged){
                printf("\033[34mD  \033[0m");      // Vert pour un drapeau
            } else {
                if(cell->probability == 1){
                    printf("\033[31m#  \033[0m");
                } else if(cell->probability == 0){
                    printf("\033[32m#  \033[0m");
                } else {
                    if(m < 9) printf("\033[36m#  \033[0m");
                    else printf("\033[36m#  \033[0m");
                }
            }
            if (j < m-1) cell = cell->adjacentCells[4];  // Déplace vers la droite
        }
        printf("\n");
        if (i < n-1) rowStart = rowStart->adjacentCells[6];  // Déplace vers le bas
    }
    printf("\n");
}

// Fonction pour libérer la mémoire du plateau
void freeBoard(Cell* board, int n, int m){
    Cell* rowStart = board;
    for (int i = 0; i < n; i++) {
        Cell* cell = rowStart;
        for (int j = 0; j < m; j++) {
            Cell* current = cell;
            if (j < m-1) cell = cell->adjacentCells[4];
            if (j != 0){
                free(current->adjacentCells);
                free(current);
            }
        }
        if (i < n-1) rowStart = rowStart->adjacentCells[6];
    }
    // Libère la mémoire de la première colonne
    Cell* cell = board;
    for (int i = 0; i < n; i++){
        Cell* current = cell;
        if (i < n-1) cell = cell->adjacentCells[6];
        free(current->adjacentCells);
        free(current);
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

bool isWon(Cell* board, int numMines, int n, int m){
    int numFlagged = 0;
    bool allRevealed = true;


    for(int i = 0; i < n; i++){
        Cell* cell = board;
        for(int j = 0; j < i; j++){
            cell = cell->adjacentCells[6];
        }
        for(int j = 0; j < m; j++){
            if(cell->isFlagged && cell->isMine){
                numFlagged++;
            }
            if(!cell->isRevealed && !cell->isMine){
                allRevealed = false;
            }
            cell = cell->adjacentCells[4];
        }
    }
    if(numFlagged == numMines || allRevealed){
        return true;
    }
    return false;
}

// Fonction principale
int main(){
    srand(time(NULL));

    int x,y;
    char action;
    int n = 16;
    int m = 30;
    int bombs = 99;

    Cell* board = initBoard(n, m);
    printf("Board initialized\n");
    bool mines_added = false;

    while(1){
        calculate_probabilities(board, n, m);
        printBoard(board, n, m);
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
            if(!mines_added){
                addMines(board, bombs, n, m, x, y);
                mines_added = true;
            }
            Cell* cell = board;
            for (int i = 0; i < x; i++) {
                cell = cell->adjacentCells[6];
            }
            for (int i = 0; i < y; i++) {
                cell = cell->adjacentCells[4];
            }
            if(cell->isMine){
                printBoard(board, n, m);
                printf("BOOM! Vous avez perdu.\n");
                exit(0);
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
        if(isWon(board, bombs, n, m)){
            printBoard(board, n, m);
            printf("Félicitations, vous avez gagné !\n");
            break;
        }
    }
    freeBoard(board, n, m);
    return 0;
}