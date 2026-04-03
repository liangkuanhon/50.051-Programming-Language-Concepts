#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "generator.h"
#include "stack.h"

/*Direction bitmask*/
#define UP    1
#define DOWN  2
#define LEFT  4
#define RIGHT 8

int dx[]       = {0, 0, -1, 1};
int dy[]       = {-1, 1, 0, 0};
int dir_bit[]  = {UP, DOWN, LEFT, RIGHT};
int opposite[] = {DOWN, UP, RIGHT, LEFT};

/* Recursive backtracker algorithm */
static void generate_maze_backtracker(Grid *grid, int seed) {
    int x, y, i;
    Stack *stack;
    Cell current;
    int cx, cy;
    int neighbors[4];
    int count;
    int dir, nx, ny;

    srand(seed);

    for (y = 0; y < grid->height; y++) {
        for (x = 0; x < grid->width; x++) {
            grid->cells[y][x]   = 0;
            grid->visited[y][x] = 0;
        }
    }

    stack = create_stack(grid->width * grid->height);

    push(stack, 0, 0);
    grid->visited[0][0] = 1;

    while (!is_empty(stack)) {
        current = peek(stack);
        cx      = current.x;
        cy      = current.y;
        count   = 0;

        for (i = 0; i < 4; i++) {
            nx = cx + dx[i];
            ny = cy + dy[i];

            if (nx >= 0 && nx < grid->width &&
                ny >= 0 && ny < grid->height &&
                !grid->visited[ny][nx]) {
                neighbors[count++] = i;
            }
        }

        if (count > 0) {
            dir = neighbors[rand() % count];

            nx = cx + dx[dir];
            ny = cy + dy[dir];

            grid->cells[cy][cx] |= dir_bit[dir];
            grid->cells[ny][nx] |= opposite[dir];

            grid->visited[ny][nx] = 1;

            push(stack, nx, ny);
        } else {
            pop(stack);
        }
    }

    free_stack(stack);
}

/* Prim's algorithm */
static void generate_maze_prim(Grid *grid, int seed) {
    int x, y, i;
    Stack *frontier;
    Cell current;
    int cx, cy;
    int neighbors[4];
    int count;
    int dir, nx, ny;
    int frontier_count;
    int random_idx;

    srand(seed);

    for (y = 0; y < grid->height; y++) {
        for (x = 0; x < grid->width; x++) {
            grid->cells[y][x]   = 0;
            grid->visited[y][x] = 0;
        }
    }

    grid->visited[0][0] = 1;

    frontier = create_stack(grid->width * grid->height);
    push(frontier, 0, 0);

    while (!is_empty(frontier)) {
        frontier_count = frontier->top + 1;
        random_idx = rand() % frontier_count;
        
        current = frontier->data[random_idx];
        cx = current.x;
        cy = current.y;

        count = 0;

        for (i = 0; i < 4; i++) {
            nx = cx + dx[i];
            ny = cy + dy[i];

            if (nx >= 0 && nx < grid->width &&
                ny >= 0 && ny < grid->height &&
                !grid->visited[ny][nx]) {
                neighbors[count++] = i;
            }
        }

        if (count > 0) {
            dir = neighbors[rand() % count];

            nx = cx + dx[dir];
            ny = cy + dy[dir];

            grid->cells[cy][cx] |= dir_bit[dir];
            grid->cells[ny][nx] |= opposite[dir];

            grid->visited[ny][nx] = 1;
            push(frontier, nx, ny);
        } else {
            if (random_idx < frontier->top) {
                frontier->data[random_idx] = frontier->data[frontier->top];
            }
            frontier->top--;
        }
    }

    free_stack(frontier);
}

void generate_maze(Grid *grid, int seed, const char *algorithm) {
    if (algorithm == NULL || strcmp(algorithm, "backtracker") == 0) {
        generate_maze_backtracker(grid, seed);
    } else if (strcmp(algorithm, "prim") == 0) {
        generate_maze_prim(grid, seed);
    } else {
        generate_maze_backtracker(grid, seed);
    }
}
