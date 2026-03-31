#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

#define MAX_LINE 256

typedef enum {
    SEC_NONE,
    SEC_MAZE,
    SEC_SOLVER
} Section;

/* --- trim whitespace (in place) --- */
static void trim(char *str) {
    char *start;
    char *end;

    start = str;
    while (isspace((unsigned char)*start)) start++;

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
}

/* --- check comment --- */
static int is_comment(const char *line) {
    return line[0] == '#' || line[0] == ';';
}

/* --- parse [section] --- */
static int parse_section(const char *line, char *section) {
    size_t len;

    len = strlen(line);
    if (len < 3 || line[0] != '[' || line[len - 1] != ']') {
        return -1;
    }

    strncpy(section, line + 1, len - 2);
    section[len - 2] = '\0';

    return 0;
}

/* --- parse key=value --- */
static int parse_key_value(char *line, char **key, char **value) {
    char *eq;

    eq = strchr(line, '=');
    if (eq == NULL) return -1;

    *eq = '\0';
    *key = line;
    *value = eq + 1;

    trim(*key);
    trim(*value);

    if (strlen(*key) == 0 || strlen(*value) == 0) {
        return -1;
    }

    return 0;
}

/* --- main parser --- */
int parse_config(const char *filename, Config *config) {
    FILE *fp;
    char line[MAX_LINE];
    char section[64];

    Section current_section;
    char *key;
    char *value;

    int line_num;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening config file");
        return -1;
    }

    memset(config, 0, sizeof(Config));
    current_section = SEC_NONE;
    line_num = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {

        line_num++;
        trim(line);

        if (strlen(line) == 0 || is_comment(line)) {
            continue;
        }

        /* --- Section --- */
        if (line[0] == '[') {

            if (parse_section(line, section) != 0) {
                fprintf(stderr, "Error (line %d): invalid section format\n", line_num);
                fclose(fp);
                return -2;
            }

            if (strcmp(section, "maze") == 0) {
                current_section = SEC_MAZE;
            } else if (strcmp(section, "solver") == 0) {
                current_section = SEC_SOLVER;
            } else {
                fprintf(stderr, "Error (line %d): unknown section [%s]\n", line_num, section);
                fclose(fp);
                return -2;
            }

            continue;
        }

        /* --- Key=Value --- */
        if (parse_key_value(line, &key, &value) != 0) {
            fprintf(stderr, "Error (line %d): invalid key=value format\n", line_num);
            fclose(fp);
            return -2;
        }

        if (current_section == SEC_NONE) {
            fprintf(stderr, "Error (line %d): key-value outside section\n", line_num);
            fclose(fp);
            return -2;
        }

        /* --- MAZE --- */
        if (current_section == SEC_MAZE) {

            if (strcmp(key, "width") == 0) {
                if (config->has_width) {
                    fprintf(stderr, "Error (line %d): duplicate width\n", line_num);
                    fclose(fp);
                    return -2;
                }
                config->width = atoi(value);
                config->has_width = 1;

            } else if (strcmp(key, "height") == 0) {
                if (config->has_height) {
                    fprintf(stderr, "Error (line %d): duplicate height\n", line_num);
                    fclose(fp);
                    return -2;
                }
                config->height = atoi(value);
                config->has_height = 1;

            } else if (strcmp(key, "seed") == 0) {
                if (config->has_seed) {
                    fprintf(stderr, "Error (line %d): duplicate seed\n", line_num);
                    fclose(fp);
                    return -2;
                }
                config->seed = atoi(value);
                config->has_seed = 1;

            } else {
                fprintf(stderr, "Error (line %d): unknown key '%s' in [maze]\n", line_num, key);
                fclose(fp);
                return -2;
            }
        }

        /* --- SOLVER --- */
        else if (current_section == SEC_SOLVER) {

            if (strcmp(key, "show_visited") == 0) {
                if (config->has_show_visited) {
                    fprintf(stderr, "Error (line %d): duplicate show_visited\n", line_num);
                    fclose(fp);
                    return -2;
                }
                config->show_visited = atoi(value);
                config->has_show_visited = 1;

            } else {
                fprintf(stderr, "Error (line %d): unknown key '%s' in [solver]\n", line_num, key);
                fclose(fp);
                return -2;
            }
        }
    }

    fclose(fp);

    /* --- Final validation --- */
    if (!config->has_width) {
        fprintf(stderr, "Error: missing required field 'width'\n");
        return -3;
    }

    if (!config->has_height) {
        fprintf(stderr, "Error: missing required field 'height'\n");
        return -3;
    }

    /* 1. Ensure dimensions are at least 3 (minimum possible maze with walls) */
    if (config->width < 5 || config->height < 5) {
        fprintf(stderr, "Error: dimensions must be at least 5x5 a viable maze.\n");
        return -2;
    }

    /* 2. Input Sanitisation
    Enforcing ODD numbers for the physical grid */
    if (config->width % 2 == 0) {
        fprintf(stderr, "Warning: width must be odd. Adjusting %d to %d.\n", config->width, config->width + 1);
        config->width++;
    }

    if (config->height % 2 == 0) {
        fprintf(stderr, "Warning: height must be odd. Adjusting %d to %d.\n", config->height, config->height + 1);
        config->height++;
    }

    /* 3. Translate Physical Input to Logical Grid for the Generator
       Example: Input 5 becomes Logical 2.
       Formula: (Physical - 1) / 2 */
    config->width = (config->width - 1) / 2;
    config->height = (config->height - 1) / 2;

    return 0;
}
