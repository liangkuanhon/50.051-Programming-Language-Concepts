#include <stdio.h>
#include <stdlib.h>
#include "config/parser.h"
#include "grid.h"
#include "maze/generator.h"
#include "solver/bfs.h"
#include "render/bmp.h"

/* allocate 2D array */
int **alloc_2d(int rows, int cols) {
    int i;
    int **arr = (int **)malloc(rows * sizeof(int *));
    for (i = 0; i < rows; i++) {
        arr[i] = (int *)malloc(cols * sizeof(int));
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

int main(void) {
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

    grid.width  = config.width;
    grid.height = config.height;

    grid.cells   = alloc_2d(grid.height, grid.width);
    grid.visited = alloc_2d(grid.height, grid.width);

    generate_maze(&grid, config.seed);

    if (solve_maze(&grid) == 0) {
        printf("Maze solved.\n");
        if (write_solved_txt(&grid, "maze_solved.txt") == 0) {
            printf("Solution written to maze_solved.txt\n");
        } else {
            printf("Failed to write maze_solved.txt\n");
        }

        if(render_maze_bmp("maze_solution.bmp", &grid, config.show_visited) == 0) {
            printf("Maze rendered to maze_solution.bmp\n");
        } else {
            printf("Failed to render maze_solution.bmp\n");
        }

    } else {
        printf("No solution found.\n");
    }

    free_2d(grid.cells,   grid.height);
    free_2d(grid.visited, grid.height);

    return 0;
}
