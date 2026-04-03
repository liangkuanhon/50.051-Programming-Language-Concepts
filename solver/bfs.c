#include <stdlib.h>
#include <stdio.h>
#include "bfs.h"
#include "queue.h"

/* Bitmask directions, matching generator.c */
#define BFS_RIGHT 8
#define BFS_DOWN  2

/* Direction vectors matching generator.c: UP, DOWN, LEFT, RIGHT */
static int dx[]      = {0,  0, -1, 1};
static int dy[]      = {-1, 1,  0, 0};
static int dir_bit[] = {1,  2,  4, 8};

/* Allocate a height x cols 2D int array, all values set to -1 */
static int **alloc_parent(int rows, int cols) {
    int i, j;
    int **arr = (int **)malloc(rows * sizeof(int *));
    for (i = 0; i < rows; i++) {
        arr[i] = (int *)malloc(cols * sizeof(int));
        for (j = 0; j < cols; j++) {
            arr[i][j] = -1;
        }
    }
    return arr;
}

static void free_parent(int **arr, int rows) {
    int i;
    for (i = 0; i < rows; i++) {
        free(arr[i]);
    }
    free(arr);
}

int solve_maze(Grid *grid) {
    int x, y, i;
    int end_x, end_y;
    int found;
    int **par_x; /* parent x-coordinate of each cell */
    int **par_y; /* parent y-coordinate of each cell */
    Queue *q;

    end_x = grid->width  - 1;
    end_y = grid->height - 1;

    /* Reset visited array: BFS takes ownership from the generator */
    for (y = 0; y < grid->height; y++) {
        for (x = 0; x < grid->width; x++) {
            grid->visited[y][x] = 0;
        }
    }

    par_x = alloc_parent(grid->height, grid->width);
    par_y = alloc_parent(grid->height, grid->width);

    q = create_queue(grid->width * grid->height);

    /* Enqueue start cell */
    enqueue(q, 0, 0);
    grid->visited[0][0] = 1;

    found = 0;

    while (!is_queue_empty(q)) {
        QCell cur = dequeue(q);
        int cx = cur.x;
        int cy = cur.y;

        if (cx == end_x && cy == end_y) {
            found = 1;
            break;
        }

        for (i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            /* Check passage exists and neighbour not yet visited */
            if ((grid->cells[cy][cx] & dir_bit[i]) &&
                nx >= 0 && nx < grid->width &&
                ny >= 0 && ny < grid->height &&
                grid->visited[ny][nx] == 0) {

                grid->visited[ny][nx] = 1;
                par_x[ny][nx] = cx;
                par_y[ny][nx] = cy;
                enqueue(q, nx, ny);
            }
        }
    }

    free_queue(q);

    if (!found) {
        free_parent(par_x, grid->height);
        free_parent(par_y, grid->height);
        return -1;
    }

    /* Trace path back from end to start and mark as 2 */
    x = end_x;
    y = end_y;
    while (!(x == 0 && y == 0)) {
        int px, py;
        grid->visited[y][x] = 2;
        px = par_x[y][x];
        py = par_y[y][x];
        x  = px;
        y  = py;
    }
    grid->visited[0][0] = 2; /* mark start cell on path */

    free_parent(par_x, grid->height);
    free_parent(par_y, grid->height);

    return 0;
}

int write_solved_txt(Grid *grid, const char *filename) {
    int rows, cols;
    int cx, cy;
    int r;
    char **text;
    FILE *fp;

    rows = 2 * grid->height + 1;
    cols = 2 * grid->width  + 1;

    /* Allocate text grid and fill with walls */
    text = (char **)malloc(rows * sizeof(char *));
    for (r = 0; r < rows; r++) {
        int c;
        text[r] = (char *)malloc((cols + 1) * sizeof(char));
        for (c = 0; c <= cols; c++) {
            text[r][c] = '#';
        }
    }

    /* Pass 1: open corridors and passages (space for unvisited, dot for path) */
    for (cy = 0; cy < grid->height; cy++) {
        for (cx = 0; cx < grid->width; cx++) {
            int ty = 2 * cy + 1;
            int tx = 2 * cx + 1;
            int on_path = (grid->visited[cy][cx] == 2);

            /* Cell position */
            text[ty][tx] = on_path ? '.' : ' ';

            /* Passage to the right */
            if ((grid->cells[cy][cx] & BFS_RIGHT) && (cx + 1 < grid->width)) {
                int right_path = (grid->visited[cy][cx + 1] == 2);
                text[ty][tx + 1] = (on_path && right_path) ? '.' : ' ';
            }

            /* Passage downward */
            if ((grid->cells[cy][cx] & BFS_DOWN) && (cy + 1 < grid->height)) {
                int down_path = (grid->visited[cy + 1][cx] == 2);
                text[ty + 1][tx] = (on_path && down_path) ? '.' : ' ';
            }
        }
    }

    /* Open entrance and exit */
    text[1][1] = '.';
    text[2 * grid->height - 1][2 * grid->width - 1] = '.';
    
    /* Mark entrance and exit */
    text[1][0] = 'S';
    text[2 * grid->height - 1][2 * grid->width] = 'E';

    /* Write to file */
    fp = fopen(filename, "w");
    if (!fp) {
        for (r = 0; r < rows; r++) free(text[r]);
        free(text);
        return -1;
    }

    for (r = 0; r < rows; r++) {
        text[r][cols] = '\0';
        fprintf(fp, "%s\n", text[r]);
        free(text[r]);
    }
    free(text);
    fclose(fp);

    return 0;
}
