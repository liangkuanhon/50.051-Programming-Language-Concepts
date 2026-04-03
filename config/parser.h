#ifndef PARSER_H
#define PARSER_H

typedef struct {
    /* [maze] */
    int  width,  has_width;
    int  height, has_height;
    int  seed,   has_seed;
    char maze_algorithm[64];
    int  has_maze_algorithm;

    /* [solver] */
    int  show_visited, has_show_visited;
    char solver_algorithm[64];
    int  has_solver_algorithm;

} Config;

/*
 * Parse config.ini
 *
 * Returns:
 *   0  -> success
 *  -1  -> file error
 *  -2  -> syntax error
 *  -3  -> missing required fields
 *  -4  -> malformed key (expected '=' not found)
 */
int parse_config(const char *filename, Config *config);
void apply_dimension_transform(Config *config);

#endif
