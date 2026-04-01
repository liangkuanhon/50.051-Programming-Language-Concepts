#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config/parser.h"
#include "grid.h"
#include "maze/generator.h"
#include "solver/bfs.h"
#include "render/bmp.h"

/* --- Usage --- */
static void print_usage(const char *prog) {
    fprintf(stderr, "  --width  <n>       Override maze width\n");
    fprintf(stderr, "  --height <n>       Override maze height\n");
    fprintf(stderr, "  --seed   <n>       Override random seed\n");
    fprintf(stderr, "  --algo   <name>    Override maze algorithm (backtracker|prim|kruskal|wilson)\n");
    fprintf(stderr, "  --solver <name>    Override solver algorithm (bfs|dfs|astar|dijkstra)\n");
    fprintf(stderr, "  --show-visited     Override show_visited to 1\n");
}

/* --- CLI Parser ---
 * Runs AFTER parse_config() so every flag is an explicit override.
 * argv[0] = binary, argv[1] = config file, argv[2..] = optional flags.
 * Values are stored as raw physical dimensions — apply_dimension_transform()
 * will handle odd-enforcement and logical conversion after this returns.
 */
static void parse_cli(int argc, char *argv[], Config *cfg) {
    int i;
    int tmp;
    char *endptr;

    for (i = 2; i < argc; i++) {

        if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            tmp = (int)strtol(argv[++i], &endptr, 10);
            if (*endptr != '\0') {
                fprintf(stderr, "Error: --width requires a number, got '%s'\n", argv[i]);
                exit(1);
            }
            cfg->width     = tmp;
            cfg->has_width = 1;

        } else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc) {
            tmp = (int)strtol(argv[++i], &endptr, 10);
            if (*endptr != '\0') {
                fprintf(stderr, "Error: --height requires a number, got '%s'\n", argv[i]);
                exit(1);
            }
            cfg->height     = tmp;
            cfg->has_height = 1;

        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            tmp = (int)strtol(argv[++i], &endptr, 10);
            if (*endptr != '\0') {
                fprintf(stderr, "Error: --seed requires a number, got '%s'\n", argv[i]);
                exit(1);
            }
            cfg->seed     = tmp;
            cfg->has_seed = 1;

        } else if (strcmp(argv[i], "--algo") == 0 && i + 1 < argc) {
            strncpy(cfg->maze_algorithm, argv[++i], sizeof(cfg->maze_algorithm) - 1);
            cfg->maze_algorithm[sizeof(cfg->maze_algorithm) - 1] = '\0';
            cfg->has_maze_algorithm = 1;

        } else if (strcmp(argv[i], "--solver") == 0 && i + 1 < argc) {
            strncpy(cfg->solver_algorithm, argv[++i], sizeof(cfg->solver_algorithm) - 1);
            cfg->solver_algorithm[sizeof(cfg->solver_algorithm) - 1] = '\0';
            cfg->has_solver_algorithm = 1;

        } else if (strcmp(argv[i], "--show-visited") == 0) {
            cfg->show_visited     = 1;
            cfg->has_show_visited = 1;

        } else {
            fprintf(stderr, "Warning: unknown flag '%s'\n", argv[i]);
        }
    }
}

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

int main(int argc, char *argv[]) {
    Config config;
    Grid grid;
    int stored_height, stored_width;
    int valid;

    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    /* 1. Parse config file — establishes all base values. */
    if (parse_config(argv[1], &config) != 0) {
        printf("Config parsing failed\n");
        return 1;
    }

    /* 2. CLI overrides — applied after, so they win over the file.
     *    Raw physical values only, no transformation yet. */
    parse_cli(argc, argv, &config);

    stored_height = config.height;
    stored_width = config.width;

    /* 3. Post-merge validation — required fields can be satisfied
     *    by either the config file or the CLI. */
    valid = 1;
    if (!config.has_width)            { fprintf(stderr, "Error: missing required field 'width'\n");                 valid = 0; }
    if (!config.has_height)           { fprintf(stderr, "Error: missing required field 'height'\n");                valid = 0; }
    if (!config.has_maze_algorithm)   { fprintf(stderr, "Error: missing required field 'algorithm' in [maze]\n");   valid = 0; }
    if (!config.has_solver_algorithm) { fprintf(stderr, "Error: missing required field 'algorithm' in [solver]\n"); valid = 0; }
    if (!valid) return 1;

    apply_dimension_transform(&config);

    grid.width  = config.width;
    grid.height = config.height;

    grid.cells   = alloc_2d(grid.height, grid.width);
    grid.visited = alloc_2d(grid.height, grid.width);

    generate_maze(&grid, config.seed);

    printf("Config: width=%d height=%d maze_algo=%s solver_algo=%s show_visited=%d\n",
           stored_width, stored_height,
           config.maze_algorithm, config.solver_algorithm,
           config.show_visited);

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
