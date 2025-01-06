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

// 0 1 2
// 3 X 4
// 5 6 7

// (0,0) (1,0)
// (0,1)