#ifndef GRID_H
#define GRID_H

typedef struct {
    int width;
    int height;

    /* 2D maze structure */
    int **cells;

    /* visited array (for BFS / rendering) */
    int **visited;

} Grid;

#endif
