#include <stdbool.h>

typedef struct Cell {
    int x;
    int y;
    int adjacentMines;
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    struct Cell** adjacentCells;

    int ai_value;
    float ai_probability;
} Cell;

// 0 1 2
// 3 X 4
// 5 6 7

// (0,0) (1,0)
// (0,1)

// Pour chaque ligne de texte i:
    // cell = origin
    // Pour _ in range i :
        // cell = cell->adjacentCells[6]
    // Pour chaque case _ :
        // print la case
        // cell = cell->adjacentCells[4]
    // Print \n