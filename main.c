#include <stdio.h>
#include <stdlib.h>
#include "config/parser.h"
#include "grid.h"
#include "maze/generator.h"

/*Debug maze visualization with # and .*/
// void print_mono_maze(Grid *grid) {
//     int h = grid->height;
//     int w = grid->width;
//     int y, x;

//     int rows = 2*h + 1;
//     int cols = 2*w + 1;

//     char **maze = malloc(rows * sizeof(char *));
//     for (y = 0; y < rows; y++) {
//         maze[y] = malloc(cols);
//         for (x = 0; x < cols; x++) {
//             maze[y][x] = '#';
//         }
//     }

//     for (y = 0; y < h; y++) {
//         for (x = 0; x < w; x++) {
//             int cy = 2*y + 1;
//             int cx = 2*x + 1;

//             maze[cy][cx] = '.';

//             int c = grid->cells[y][x];
//             if (c & 1) maze[cy-1][cx] = '.';
//             if (c & 2) maze[cy+1][cx] = '.';
//             if (c & 4) maze[cy][cx-1] = '.';
//             if (c & 8) maze[cy][cx+1] = '.';
//         }
//     }

//     for (y = 0; y < rows; y++) {
//         for (x = 0; x < cols; x++) {
//             putchar(maze[y][x]);
//         }
//         putchar('\n');
//     }

//     for (y = 0; y < rows; y++) free(maze[y]);
//     free(maze);
// }

/* STUBS (temporary) remove it when implemented */

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
    print_mono_maze(&grid);
    // solve_maze(&grid);
    // render_maze("output.bmp", &grid);

    free_2d(grid.cells, grid.height);
    free_2d(grid.visited, grid.height);

    return 0;
}