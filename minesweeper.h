#include <stdbool.h>

typedef struct {
    int x;
    int y;
    int adjacentMines;
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    Cell** adjacentCells;
} Cell;