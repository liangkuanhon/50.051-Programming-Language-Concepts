#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

/* --- 1. States and Typedefs --- */
typedef enum {
    STATE_START,
    STATE_SECTION,
    STATE_KEY,
    STATE_BEFORE_EQ,
    STATE_VALUE,
    STATE_COMMENT
} ParserState;

typedef enum { SEC_NONE, SEC_MAZE, SEC_SOLVER } Section;

/* Value type union: the parser now speaks int, float, and string */
typedef enum { VAL_INT, VAL_FLOAT, VAL_STRING } ValueType;

typedef union {
    int   as_int;
    float as_float;
    char  as_str[64];
} ParsedValue;

typedef void (*SettingHandler)(const ParsedValue *pv, Config *cfg);

typedef struct {
    const char    *key_name;
    ValueType      expected_type;
    SettingHandler handler;
    Section        section;
} SettingMap;

/* --- 2. Robust Numeric Parsers --- */
static int safe_atoi(const char *str, int *result) {
    char *endptr;
    long val = strtol(str, &endptr, 10);
    if (endptr == str) return 0;
    while (*endptr) { if (!isspace((unsigned char)*endptr++)) return 0; }
    *result = (int)val;
    return 1;
}

static int safe_atof(const char *str, float *result) {
    char *endptr;
    double val = strtod(str, &endptr);
    if (endptr == str) return 0;
    while (*endptr) { if (!isspace((unsigned char)*endptr++)) return 0; }
    *result = (float)val;
    return 1;
}

/* --- 3. Generic Value Coercer ---
 * Attempts to parse raw string into the expected ValueType.
 * Returns 0 on failure, 1 on success.
 */
static int coerce_value(const char *raw, ValueType type, ParsedValue *out) {
    switch (type) {
        case VAL_INT:    return safe_atoi(raw, &out->as_int);
        case VAL_FLOAT:  return safe_atof(raw, &out->as_float);
        case VAL_STRING:
            strncpy(out->as_str, raw, sizeof(out->as_str) - 1);
            out->as_str[sizeof(out->as_str) - 1] = '\0';
            return 1;
    }
    return 0;
}

/* --- 4. Enum Validator Helper ---
 * Checks that a string value is one of an allowed set.
 */
static int validate_enum(const char *val, const char **allowed, int count) {
    int i;
    for (i = 0; i < count; i++)
        if (strcmp(val, allowed[i]) == 0) return 1;
    return 0;
}

/* --- 5. Action Handlers --- */
static void handle_width(const ParsedValue *pv, Config *cfg) {
    cfg->width = pv->as_int;
    cfg->has_width = 1;
}

static void handle_height(const ParsedValue *pv, Config *cfg) {
    cfg->height = pv->as_int;
    cfg->has_height = 1;
}

static void handle_seed(const ParsedValue *pv, Config *cfg) {
    cfg->seed = pv->as_int;
    cfg->has_seed = 1;
}

static void handle_show(const ParsedValue *pv, Config *cfg) {
    int v = pv->as_int;
    if (v != 0 && v != 1) {
        fprintf(stderr, "Error: show_visited must be 0 or 1, got %d.\n", v);
        exit(1);
    }
    cfg->show_visited = v;
    cfg->has_show_visited = 1;
}

static void handle_maze_algorithm(const ParsedValue *pv, Config *cfg) {
    static const char *allowed[] = { "backtracker", "prim", "kruskal", "wilson" };
    if (!validate_enum(pv->as_str, allowed, 4)) {
        fprintf(stderr, "Error: Unknown maze algorithm '%s'.\n"
                        "       Valid options: backtracker, prim, kruskal, wilson\n",
                pv->as_str);
        exit(1);
    }
    strncpy(cfg->maze_algorithm, pv->as_str, sizeof(cfg->maze_algorithm) - 1);
    cfg->has_maze_algorithm = 1;
}

static void handle_solver_algorithm(const ParsedValue *pv, Config *cfg) {
    static const char *allowed[] = { "bfs", "dfs", "astar", "dijkstra" };
    if (!validate_enum(pv->as_str, allowed, 4)) {
        fprintf(stderr, "Error: Unknown solver algorithm '%s'.\n"
                        "       Valid options: bfs, dfs, astar, dijkstra\n",
                pv->as_str);
        exit(1);
    }
    strncpy(cfg->solver_algorithm, pv->as_str, sizeof(cfg->solver_algorithm) - 1);
    cfg->has_solver_algorithm = 1;
}

/* --- 6. Dispatch Table ---
 * Each row binds: key name -> expected type -> handler -> owning section.
 * Adding a new config field = adding one row here + one handler above.
 * Note: "algorithm" appears twice, disambiguated by section.
 */
static SettingMap dispatch[] = {
    { "width",        VAL_INT,    handle_width,            SEC_MAZE   },
    { "height",       VAL_INT,    handle_height,           SEC_MAZE   },
    { "seed",         VAL_INT,    handle_seed,             SEC_MAZE   },
    { "algorithm",    VAL_STRING, handle_maze_algorithm,   SEC_MAZE   },
    { "show_visited", VAL_INT,    handle_show,             SEC_SOLVER },
    { "algorithm",    VAL_STRING, handle_solver_algorithm, SEC_SOLVER },
};

static void process_token(const char *key, const char *raw_val, Section current_sec, Config *cfg) {
    int i;
    ParsedValue pv;
    int n;
    
    n = (int)(sizeof(dispatch) / sizeof(dispatch[0]));

    for (i = 0; i < n; i++) {
        /* Disambiguate by checking both section and key name */
        if (dispatch[i].section != current_sec) continue;
        if (strcmp(key, dispatch[i].key_name) != 0) continue;

        /* Attempt to convert the raw string into the type defined in dispatch table */
        if (!coerce_value(raw_val, dispatch[i].expected_type, &pv)) {
            fprintf(stderr, "Error: Value '%s' for key '%s' is not the expected type.\n",
                    raw_val, key);
            exit(1);
        }
        
        /* Call the specific handler (e.g., handle_width) */
        dispatch[i].handler(&pv, cfg);
        return;
    }
    
    fprintf(stderr, "Warning: Unknown key '%s' in current section.\n", key);
}

/* --- 7. Right-trim helper ---
 * Strips trailing whitespace in-place. Used when flushing the value
 * buffer, since we now accumulate inner spaces faithfully.
 */
static void rtrim(char *s) {
    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        s[--len] = '\0';
}

/* --- 8. The FSM Stream Parser --- */
int parse_config(const char *filename, Config *config) {
    FILE *fp;
    int ch;
    char key[64], val[128], sec_name[64];
    int k_idx = 0, v_idx = 0, s_idx = 0;
    int line = 1;   /* line counter for error messages */

    ParserState state = STATE_START;
    Section current_sec = SEC_NONE;

    fp = fopen(filename, "r");
    if (!fp) { perror("Error opening config file"); return -1; }

    memset(config, 0, sizeof(Config));

    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') line++;

        switch (state) {
            case STATE_START:
                if (ch == '[')                   { s_idx = 0; state = STATE_SECTION; }
                else if (ch == '#' || ch == ';') state = STATE_COMMENT;
                else if (isalpha(ch))            { k_idx = 0; key[k_idx++] = ch; state = STATE_KEY; }
                break;

            case STATE_SECTION:
                if (ch == ']') {
                    sec_name[s_idx] = '\0';
                    if      (strcmp(sec_name, "maze")   == 0) current_sec = SEC_MAZE;
                    else if (strcmp(sec_name, "solver") == 0) current_sec = SEC_SOLVER;
                    else {
                        fprintf(stderr, "Warning: Unknown section '[%s]' at line %d.\n",
                                sec_name, line);
                        current_sec = SEC_NONE;
                    }
                    state = STATE_START;
                } else if (!isspace(ch)) {
                    /* Normalise section names to lowercase: [Maze] == [maze] */
                    if (s_idx < 63) sec_name[s_idx++] = (char)tolower(ch);
                }
                break;

            case STATE_KEY:
                if (ch == '=')        { key[k_idx] = '\0'; v_idx = 0; state = STATE_VALUE; }
                else if (isspace(ch)) { key[k_idx] = '\0'; state = STATE_BEFORE_EQ; }
                else                  { if (k_idx < 63) key[k_idx++] = (char)tolower(ch); }
                break;

            case STATE_BEFORE_EQ:
                if (ch == '=') { v_idx = 0; state = STATE_VALUE; }
                else if (!isspace(ch)) {
                    fprintf(stderr, "Error: Expected '=' after key '%s' at line %d.\n",
                            key, line);
                    fclose(fp);
                    return -4;
                }
                break;

            case STATE_VALUE:
                if (ch == '\n' || ch == '#' || ch == ';') {
                    val[v_idx] = '\0';
                    rtrim(val);
                    if (current_sec != SEC_NONE)
                        process_token(key, val, current_sec, config);
                    state = (ch == '\n') ? STATE_START : STATE_COMMENT;
                } else {
                    if (v_idx == 0 && isspace(ch)) break; /* skip leading whitespace */
                    if (v_idx < 127) val[v_idx++] = (char)ch;
                }
                break;

            case STATE_COMMENT:
                if (ch == '\n') state = STATE_START;
                break;
        }
    }

    /* Flush a value that ends at EOF without a trailing newline */
    if (state == STATE_VALUE) {
        val[v_idx] = '\0';
        rtrim(val);
        if (current_sec != SEC_NONE)
            process_token(key, val, current_sec, config);
    }

    fclose(fp);

    /* --- 9. Post-Parse Validation --- */
    if (!config->has_width) {
        fprintf(stderr, "Error: missing required field 'width'\n");
        return -3;
    }

    if (!config->has_height) {
        fprintf(stderr, "Error: missing required field 'height'\n");
        return -3;
    }

    /* Error for algorithms if omitted from file */
    if (!config->has_maze_algorithm) {
        fprintf(stderr, "Error: missing required field 'Maze Algorithm: 'backtracker' or 'prim''\n");
        return -3;
    }

    if (!config->has_solver_algorithm) {
        fprintf(stderr, "Error: missing required field 'Solver Algorithm: 'bfs' or 'dfs''\n");
        return -3;
    }

    /* Ensure dimensions are at least 3 (minimum possible maze with walls) */
    if (config->width < 5 || config->height < 5) {
        fprintf(stderr, "Error: dimensions must be at least 5x5 a viable maze.\n");
        return -2;
    }

    if (config->width % 2 == 0) {
        fprintf(stderr, "Adjusting width %d -> %d (must be odd).\n",
                config->width, config->width + 1);
        config->width++;
    }
    if (config->height % 2 == 0) {
        fprintf(stderr, "Adjusting height %d -> %d (must be odd).\n",
                config->height, config->height + 1);
        config->height++;
    }

    return 0;
}

void apply_dimension_transform(Config *config) {

    config->width  = (config->width  - 1) / 2;
    config->height = (config->height - 1) / 2;
}

