#include <stdlib.h>
#include <time.h>
#include "generator.h"
#include "stack.h"

/*Direction bitmask*/
#define UP    1
#define DOWN  2
#define LEFT  4
#define RIGHT 8

int dx[] = {0, 0, -1, 1};
int dy[] = {-1, 1, 0, 0};
int dir_bit[] = {UP, DOWN, LEFT, RIGHT};
int opposite[] = {DOWN, UP, RIGHT, LEFT};

void generate_maze(Grid *grid, int seed) {
    int x, y, i;

    srand(seed);

    for (y = 0; y < grid->height; y++) {
        for (x = 0; x < grid->width; x++) {
            grid->cells[y][x] = 0;
            grid->visited[y][x] = 0;
        }
    }

    Stack *stack = create_stack(grid->width * grid->height);

    push(stack, 0, 0);
    grid->visited[0][0] = 1;

    while (!is_empty(stack)) {
        Cell current = peek(stack);
        int cx = current.x;
        int cy = current.y;

        int neighbors[4];
        int count = 0;

        for (i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (nx >= 0 && nx < grid->width &&
                ny >= 0 && ny < grid->height &&
                !grid->visited[ny][nx]) {
                neighbors[count++] = i;
            }
        }

        if (count > 0) {
            int dir = neighbors[rand() % count];

            int nx = cx + dx[dir];
            int ny = cy + dy[dir];

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