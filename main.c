#include <stdio.h>
#include <stdlib.h>
#include "config/parser.h"
#include "grid.h"

/* STUBS (temporary) remove it when implemented */
void generate_maze(Grid *grid, int seed) {
    printf("[STUB] generate_maze called (seed=%d)\n", seed);
}

void solve_maze(Grid *grid) {
    printf("[STUB] solve_maze called\n");
}

void render_maze(const char *filename, Grid *grid) {
    printf("[STUB] render_maze called → %s\n", filename);
}

/* allocate 2D array */
int **alloc_2d(int rows, int cols) {
    int i;
    int **arr = malloc(rows * sizeof(int *));
    for (i = 0; i < rows; i++) {
        arr[i] = malloc(cols * sizeof(int));
    }
    return arr;
}

/* free 2D array */
void free_2d(int **arr, int rows) {
    int i;
    for (i = 0; i < rows; i++) {
        free(arr[i]);
    }
    free(arr);
}

int main() {
    Config config;
    Grid grid;

    if (parse_config("config.ini", &config) != 0) {
        printf("Config parsing failed\n");
        return 1;
    }

    printf("Parsed config:\n");
    printf("width=%d height=%d seed=%d show_visited=%d\n",
           config.width, config.height,
           config.seed, config.show_visited);

    grid.width = config.width;
    grid.height = config.height;

    grid.cells = alloc_2d(grid.height, grid.width);
    grid.visited = alloc_2d(grid.height, grid.width);

    generate_maze(&grid, config.seed);
    solve_maze(&grid);
    render_maze("output.bmp", &grid);

    free_2d(grid.cells, grid.height);
    free_2d(grid.visited, grid.height);

    return 0;
}