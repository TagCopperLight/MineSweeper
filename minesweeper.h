#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <stdbool.h>

typedef struct Cell {
    int x;
    int y;
    int adjacentMines;
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    struct Cell** adjacentCells;

    float probability;
} Cell;

#endif // MINESWEEPER_H