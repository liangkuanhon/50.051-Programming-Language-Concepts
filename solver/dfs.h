#ifndef DFS_H
#define DFS_H

#include "../grid.h"

/*
 * solve_maze_dfs: solve the maze stored in *grid using iterative
 * Depth-First Search (DFS) with an explicit stack.
 *
 * Start: (0, 0)
 * End:   (grid->width - 1, grid->height - 1)
 *
 * Passages between cells are encoded in grid->cells as bitmasks:
 *   UP=1  DOWN=2  LEFT=4  RIGHT=8
 *
 * DFS does NOT guarantee the shortest path. It finds a valid path
 * by following the first available direction at each cell and
 * backtracking when no unvisited neighbours remain.
 *
 * On return, grid->visited is populated as follows:
 *   0 -> cell was not reached during DFS
 *   1 -> cell was explored but is not on the solution path
 *   2 -> cell is on the solution path from start to end
 *
 * Returns:
 *   0  -> solution found
 *  -1  -> no path exists
 */
int solve_maze_dfs(Grid *grid);

#endif
