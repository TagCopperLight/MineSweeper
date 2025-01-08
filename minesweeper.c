#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "minesweeper.h"

// Fonction pour initialiser le plateau de jeu
Cell* initBoard(int size){
    // Alloue de la mémoire pour le tableau 3D de cellules
    Cell*** board = malloc(size * sizeof(Cell**));
    for(int i = 0; i < size; i++){
        board[i] = malloc(size * sizeof(Cell*));
        for(int j = 0; j < size; j++){
            // Initialise chaque cellule
            board[i][j] = malloc(sizeof(Cell));
            board[i][j]->x = i;
            board[i][j]->y = j;
            board[i][j]->adjacentMines = 0;
            board[i][j]->isMine = false;
            board[i][j]->isRevealed = false;
            board[i][j]->isFlagged = false;
            board[i][j]->adjacentCells = malloc(8 * sizeof(Cell*));
            for(int k = 0; k < 8; k++){
                board[i][j]->adjacentCells[k] = NULL;
            }
        }
    }

    // Établit les liens entre les cellules adjacentes
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            if(i-1 >= 0 && j - 1 >= 0) board[i][j]->adjacentCells[0] = board[i - 1][j - 1];
            if(j-1 >= 0) board[i][j]->adjacentCells[1] = board[i][j - 1];
            if(i+1 < size && j - 1 >= 0) board[i][j]->adjacentCells[2] = board[i + 1][j - 1];
            if(i-1 >= 0) board[i][j]->adjacentCells[3] = board[i - 1][j];
            if(i + 1 < size) board[i][j]->adjacentCells[4] = board[i + 1][j];
            if(i - 1 >= 0 && j + 1 < size) board[i][j]->adjacentCells[5] = board[i - 1][j + 1];
            if(j + 1 < size) board[i][j]->adjacentCells[6] = board[i][j + 1];
            if(i + 1 < size && j + 1 < size) board[i][j]->adjacentCells[7] = board[i + 1][j + 1];
        }
    }

    // Stocke la cellule d'origine et libère la mémoire du tableau 3D
    Cell* origin = board[0][0];
    for(int i = 0; i < size; i++){free(board[i]);}
    free(board);
    return origin;
}

// Fonction pour ajouter des mines au plateau
void addMines (Cell* origin, int numMines, int size){
    for(int i = 0; i < numMines; i++){
        int x = rand() % size;
        int y = rand() % size;

        Cell* cell = origin;

        // Navigue jusqu'à la cellule cible
        for(int j = 0; j < x; j++){
            cell = cell->adjacentCells[4];
        }
        for(int j = 0; j < y; j++){
            cell = cell->adjacentCells[6];
        }

        // Place une mine et incrémente le compteur des cellules adjacentes
        cell->isMine = true;
        for(int j=0; j <8;j++){
            if(cell->adjacentCells[j]!=NULL){
                cell->adjacentCells[j]->adjacentMines++;
            }
        }
    }
}

// Fonction pour afficher le plateau
void printBoard(Cell* origin, int size) {
    printf("   ");
    for (int i = 0; i < size; i++) {
        printf("%2d ", i);  // Affiche les numéros de colonnes
    }
    printf("\n");

    Cell* rowStart = origin;
    for (int i = 0; i < size; i++) {
        printf("%2d ", i);  // Affiche les numéros de lignes
        Cell* cell = rowStart;
        for (int j = 0; j < size; j++) {
            if(cell->isRevealed){
                if(cell->isMine){
                    printf("\033[31m# \033[0m");  // Rouge pour une mine révélée
                } else if(cell->adjacentMines > 0){
                    printf("%d  ", cell->adjacentMines);
                } else {
                    printf("\033[0m  ");          // Blanc pour une case vide
                }
            } else if(cell->isFlagged){
                printf("\033[32mD \033[0m");      // Vert pour un drapeau
            } else {
                printf("\033[36m# \033[0m");      // Cyan pour une case non révélée
            }
            if (j < size-1) cell = cell->adjacentCells[6];  // Déplace vers la droite
        }
        printf("\n");
        if (i < size-1) rowStart = rowStart->adjacentCells[4];  // Déplace vers le bas
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

// Fonction principale
int main(){
    int size=10;
    int x,y;
    char action;

    Cell* board = initBoard(size);
    printf("Board initialized\n");
    addMines(board, size, 10);
    printf("Mines added\n");

    while(1){
        printBoard(board, size);
        printf("Entrez l'action (r pour révéler, f pour drapeau, q pour quitter): ");
        scanf(" %c", &action);
        if (action == 'q') {
            printf("Merci d'avoir joué !\n");
            break;
        }

        printf("Entrez les coordonnées (x y): ");
        scanf("%d %d", &x, &y);
        if (x < 0 || x >= size || y < 0 || y >= size) {
            printf("Coordonnées invalides.\n");
            continue;
        }
    }
    return 0;
}