#ifndef BFS_H
#define BFS_H

#include "../grid.h"

/*
 * solve_maze: solve the maze stored in *grid using Breadth-First Search.
 *
 * Start: (0, 0)
 * End:   (grid->width - 1, grid->height - 1)
 *
 * Passages between cells are encoded in grid->cells as bitmasks:
 *   UP=1  DOWN=2  LEFT=4  RIGHT=8
 *
 * On return, grid->visited is populated as follows:
 *   0 -> cell was not reached during BFS
 *   1 -> cell was explored but is not on the shortest path
 *   2 -> cell is on the shortest path from start to end
 *
 * Returns:
 *   0  -> solution found
 *  -1  -> no path exists
 */
int solve_maze(Grid *grid);

/*
 * write_solved_txt: write the solved maze to a plain-text file.
 *
 * The maze is expanded to a (2*width+1) x (2*height+1) character grid:
 *   '#' -> wall
 *   '.' -> solution path (cell or passage between two path cells)
 *   ' ' -> open corridor not on the solution path
 *
 * Must be called after solve_maze() so that grid->visited is populated.
 *
 * Returns:
 *   0  -> file written successfully
 *  -1  -> file could not be opened
 */
int write_solved_txt(Grid *grid, const char *filename);

#endif
