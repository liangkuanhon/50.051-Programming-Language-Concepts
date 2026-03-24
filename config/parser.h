#ifndef PARSER_H
#define PARSER_H

typedef struct {
    int width;
    int height;
    int seed;
    int show_visited;

    /*internal flags to track what was parsed*/
    int has_width;
    int has_height;
    int has_seed;
    int has_show_visited;

} Config;

/*
 * Parse config.ini
 *
 * Returns:
 *   0  -> success
 *  -1  -> file error
 *  -2  -> syntax error
 *  -3  -> missing required fields
 */
int parse_config(const char *filename, Config *config);

#endif