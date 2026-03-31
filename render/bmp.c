#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

/* Maze direction bitmasks */
#define BMP_UP 1
#define BMP_DOWN 2
#define BMP_LEFT 4
#define BMP_RIGHT 8

/* Each logical maze tile is expanded into a TILE_SIZE x TILE_SIZE pixel block */
#define TILE_SIZE 10

/* Tile categories used in the intermediate visual map */
#define TILE_WALL 0
#define TILE_OPEN 1
#define TILE_VISITED 2 
#define TILE_PATH 3
#define TILE_START 4
#define TILE_END 5

/* Simple RGB pixel type */
typedef struct {
    unsigned char b, g, r;
} Pixel;

/* Write a 16-bit unsigned integer in little-endian order */
static void write_u16_le(FILE *fp, unsigned short value) {
    fputc(value & 0xFF, fp);
    fputc((value >> 8) & 0xFF, fp);
}

/* Write a 32-bit unsigned integer in little-endian order */
static void write_u32_le(FILE *fp, unsigned int value) {
    fputc(value & 0xFF, fp);
    fputc((value >> 8) & 0xFF, fp);
    fputc((value >> 16) & 0xFF, fp);
    fputc((value >> 24) & 0xFF, fp);
}

/* Allocate a rows x cols integer grid and initialize all entries to 'value' */
static int **alloc_int_grid(int rows, int cols, int value){
    int i, j;
    int **arr = (int **)malloc(rows * sizeof(int *));

    if (arr == NULL) {
        return NULL;
    }

    for (i = 0; i < rows; i++){
        arr[i] = (int *)malloc(cols * sizeof(int));
        if (arr[i] == NULL) {
            int k;
            for (k = 0;k < i; k++){
                free(arr[k]);
            }
            free(arr);
            return NULL;
        }

        for (j=0; j < cols; j++){
            arr[i][j] = value;
        }
    }

    return arr;
}

/* Free a 2D integer grid allocated by alloc_int_grid */
static void free_int_grid(int **arr, int rows){
    int i;
    for (i=0; i < rows; i++){
        free(arr[i]);
    }
    free(arr);
}

/* Convert tile type into RGB color
 * WALL -> black
 * OPEN -> white
 * VISITED -> light blue
 * PATH -> red
 * START & END INDICATOR -> green
 */

static Pixel tile_to_color(int tile){
    Pixel p;

    switch(tile){
        case TILE_WALL:
            p.r = 0; p.g = 0; p.b = 0;
            break;
        case TILE_OPEN:
            p.r = 255; p.g = 255; p.b = 255;
            break;
        case TILE_VISITED:
            p.r = 173; p.g = 216; p.b = 230;
            break;
        case TILE_PATH:
            p.r = 255; p.g = 0; p.b = 0;
            break;
        case TILE_START:
        case TILE_END:
            p.r = 0; p.g = 255; p.b = 0;
            break;
        default:
            p.r = 255; p.g = 0; p.b = 255;
    }
    return p;
}

/* Build an intermediate tile map of size: (2 * width + 1) x (2 * height + 1) */
static int **build_tile_map(Grid *grid, int show_visited, int *out_rows, int *out_cols){
    int rows, cols;
    int **tiles;
    int cx, cy;

    rows = 2 * grid->height + 1;
    cols = 2 * grid->width + 1;

    tiles = alloc_int_grid(rows, cols, TILE_WALL);
    if (tiles == NULL) {
        return NULL;
    }

    tiles[1][0] = TILE_START;
    tiles[rows - 2][cols - 1] = TILE_END;

    for (cy = 0; cy < grid->height; cy++){
        for (cx = 0; cx < grid->width; cx++){
            int ty, tx;
            int state;

            ty = 2 * cy + 1;
            tx = 2 * cx + 1;

            if (grid->visited[cy][cx] == 2){
                state = TILE_PATH;
            } else if (grid->visited[cy][cx] == 1 && show_visited){
                state = TILE_VISITED;
            } else {
                state = TILE_OPEN;
            }

            tiles[ty][tx] = state;

            if ((grid->cells[cy][cx] & BMP_RIGHT) && (cx + 1 < grid->width)){
                int next_state;

                if (grid->visited[cy][cx] == 2 && grid->visited[cy][cx + 1] == 2) {
                    next_state = TILE_PATH;
                } else if (show_visited &&
                           (grid->visited[cy][cx] != 0 || grid->visited[cy][cx + 1] != 0)) {
                    next_state = TILE_VISITED;
                } else {
                    next_state = TILE_OPEN;
                }

                tiles[ty][tx + 1] = next_state;
            }

            if ((grid->cells[cy][cx] & BMP_DOWN) && (cy + 1 < grid->height)) {
                int next_state;

                if (grid->visited[cy][cx] == 2 && grid->visited[cy + 1][cx] == 2) {
                    next_state = TILE_PATH;
                } else if (show_visited &&
                           (grid->visited[cy][cx] != 0 || grid->visited[cy + 1][cx] != 0)) {
                    next_state = TILE_VISITED;
                } else {
                    next_state = TILE_OPEN;
                }

                tiles[ty + 1][tx] = next_state;
            }
        }
    }

    *out_rows = rows;
    *out_cols = cols;
    return tiles;
}

/* Write the 24-bit BMP headers */
/* BMP layout: 14-byte file header, 40-byte DIB header, pixel array*/
static void write_bmp_headers(FILE *fp, int width, int height, int padded_row_size) {
    unsigned int pixel_data_size;
    unsigned int file_size;

    pixel_data_size = padded_row_size * height;
    file_size = 14 + 40 + pixel_data_size;

    /* --- BMP FILE HEADER (14 bytes) --- */
    fputc('B', fp);
    fputc('M', fp);
    write_u32_le(fp, file_size);
    write_u16_le(fp, 0);
    write_u16_le(fp, 0);
    write_u32_le(fp, 14 + 40);

    /* --- DIB HEADER: BITMAPINFOHEADER (40 bytes) --- */
    write_u32_le(fp, 40);                 /* header size */
    write_u32_le(fp, (unsigned int)width);
    write_u32_le(fp, (unsigned int)height);
    write_u16_le(fp, 1);                  /* planes */
    write_u16_le(fp, 24);                 /* bits per pixel */
    write_u32_le(fp, 0);                  /* compression = BI_RGB */
    write_u32_le(fp, pixel_data_size);
    write_u32_le(fp, 2835);               /* horizontal resolution (~72 DPI) */
    write_u32_le(fp, 2835);               /* vertical resolution (~72 DPI) */
    write_u32_le(fp, 0);                  /* colors in palette */
    write_u32_le(fp, 0);                  /* important colors */
}

/* Render the tile map into BMP pixel rows*/
static int write_bmp_pixels(FILE *fp, int **tiles, int tile_rows, int tile_cols) {
    int img_width, img_height;
    int raw_row_size, padded_row_size;
    unsigned char *row_buf;
    int py, px;

    img_width = tile_cols * TILE_SIZE;
    img_height = tile_rows * TILE_SIZE;

    raw_row_size = img_width * 3;
    padded_row_size = (raw_row_size + 3) & ~3;

    write_bmp_headers(fp, img_width, img_height, padded_row_size);

    row_buf = (unsigned char *)malloc(padded_row_size);
    if (row_buf == NULL) {
        return -1;
    }

    for (py = img_height - 1; py >= 0; py--) {
        int tile_y = py / TILE_SIZE;

        memset(row_buf, 0, padded_row_size);

        for (px = 0; px < img_width; px++) {
            int tile_x = px / TILE_SIZE;
            Pixel color = tile_to_color(tiles[tile_y][tile_x]);
            int offset = px * 3;

            row_buf[offset + 0] = color.b;
            row_buf[offset + 1] = color.g;
            row_buf[offset + 2] = color.r;
        }

        if (fwrite(row_buf, 1, padded_row_size, fp) != (size_t)padded_row_size) {
            free(row_buf);
            return -1;
        }
    }

    free(row_buf);
    return 0;
}

int render_maze_bmp(const char *filename, Grid *grid, int show_visited) {
    FILE *fp;
    int **tiles;
    int tile_rows, tile_cols;
    int rc;

    tiles = build_tile_map(grid, show_visited, &tile_rows, &tile_cols);
    if (tiles == NULL) {
        return -1;
    }

    fp = fopen(filename, "wb");
    if (fp == NULL) {
        free_int_grid(tiles, tile_rows);
        return -1;
    }

    rc = write_bmp_pixels(fp, tiles, tile_rows, tile_cols);

    fclose(fp);
    free_int_grid(tiles, tile_rows);

    return rc;
}
