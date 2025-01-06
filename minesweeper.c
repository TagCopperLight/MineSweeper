#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "minesweeper.h"

Cell* initBoard(int size, int numMines){
    Cell*** board = malloc(size * sizeof(Cell**));
    for(int i = 0; i < size; i++){
        board[i] = malloc(size * sizeof(Cell*));
        for(int j = 0; j < size; j++){
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

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            if(i - 1 >= 0 && j - 1 >= 0) board[i][j]->adjacentCells[0] = board[i - 1][j - 1];
            if(j - 1 >= 0) board[i][j]->adjacentCells[1] = board[i][j - 1];
            if(i + 1 < size && j - 1 >= 0) board[i][j]->adjacentCells[2] = board[i + 1][j - 1];
            if(i - 1 >= 0) board[i][j]->adjacentCells[3] = board[i - 1][j];
            if(i + 1 < size) board[i][j]->adjacentCells[4] = board[i + 1][j];
            if(i - 1 >= 0 && j + 1 < size) board[i][j]->adjacentCells[5] = board[i - 1][j + 1];
            if(j + 1 < size) board[i][j]->adjacentCells[6] = board[i][j + 1];
            if(i + 1 < size && j + 1 < size) board[i][j]->adjacentCells[7] = board[i + 1][j + 1];
        }
    }

    Cell* origin = board[0][0];

    for(int i = 0; i < size; i++){
        free(board[i]);
    }
    free(board);
    
    return origin;
}

int main(){
    Cell* board = initBoard(5, 5);
    printf("Board initialized\n");
    free(board);

    return 0;
}