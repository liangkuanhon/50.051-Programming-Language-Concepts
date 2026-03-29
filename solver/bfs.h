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

#endif
