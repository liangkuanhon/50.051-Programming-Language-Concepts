# Part 3 — BFS Solver & Queue
---

## Overview

The solver finds the shortest path through a generated maze using **Breadth-First Search (BFS)**. It reads the maze connectivity from the shared `Grid` struct and writes its results back into `grid->visited` for the renderer to consume.

- **Entry point:** `solve_maze(Grid *grid)` in `solver/bfs.c`
- **Start cell:** `(0, 0)` — top-left corner
- **End cell:** `(width - 1, height - 1)` — bottom-right corner

---

## Files

| File | Purpose |
|------|---------|
| `solver/queue.h` | Queue interface |
| `solver/queue.c` | Queue implementation (circular buffer) |
| `solver/bfs.h` | BFS solver interface |
| `solver/bfs.c` | BFS solver implementation |

---

## Data Structure: Queue

### Design

The queue is a **circular-buffer FIFO** backed by a dynamically allocated array of `QCell` values.

```
capacity = 6
data:  [ (1,0) | (0,1) |  ...  |  ...  |  ...  |  ...  ]
        ^front                             ^rear
```

Wrapping is achieved with modular arithmetic:

```
rear  = (rear  + 1) % capacity   /* after enqueue */
front = (front + 1) % capacity   /* after dequeue */
```

This avoids shifting elements and keeps both `enqueue` and `dequeue` at **O(1)**.

### Struct

```c
typedef struct {
    int x;
    int y;
} QCell;

typedef struct {
    QCell *data;
    int    front;
    int    rear;
    int    size;
    int    capacity;
} Queue;
```

`QCell` is defined independently in `queue.h` to keep the solver module decoupled from `maze/stack.h`.

### Operations

| Function | Description |
|----------|-------------|
| `create_queue(capacity)` | Allocate a new queue. Capacity should be `width * height`. |
| `enqueue(q, x, y)` | Add cell `(x, y)` at the rear. |
| `dequeue(q)` | Remove and return the front cell. |
| `is_queue_empty(q)` | Returns 1 if `size == 0`. |
| `free_queue(q)` | Frees internal data array and the queue struct. |

---

## Algorithm: Breadth-First Search

BFS guarantees the **shortest path** in an unweighted graph. Each maze cell is a node; two cells are connected if there is an open passage between them (no wall).

### Maze Connectivity

Passages are encoded in `grid->cells[y][x]` as a bitmask:

| Bit | Direction | Value |
|-----|-----------|-------|
| 0   | UP        | 1     |
| 1   | DOWN      | 2     |
| 2   | LEFT      | 4     |
| 3   | RIGHT     | 8     |

To test whether a passage exists in direction `i` from cell `(cx, cy)`:

```c
grid->cells[cy][cx] & dir_bit[i]
```

Direction offset arrays (matching `generator.c`):

```c
static int dx[]      = {0,  0, -1, 1};   /* UP, DOWN, LEFT, RIGHT */
static int dy[]      = {-1, 1,  0, 0};
static int dir_bit[] = {1,  2,  4, 8};
```

### Step-by-Step

**1. Reset the visited array**

The generator leaves `grid->visited` fully populated (all `1`s) after maze generation. The solver takes ownership and clears it:

```c
for (y = 0; y < grid->height; y++)
    for (x = 0; x < grid->width; x++)
        grid->visited[y][x] = 0;
```

**2. Allocate parent arrays**

Two 2D arrays, `par_x` and `par_y`, record where each cell was reached from. Both are initialised to `-1`.

```
par_x[ny][nx] = cx   /* the x-coordinate of the cell that enqueued (nx, ny) */
par_y[ny][nx] = cy
```

These are used after BFS to walk the path backward from end to start.

**3. Enqueue the start cell**

```c
enqueue(q, 0, 0);
grid->visited[0][0] = 1;
```

**4. BFS loop**

```
while queue is not empty:
    cur = dequeue()
    if cur == end: stop (found = 1)
    for each direction i in {UP, DOWN, LEFT, RIGHT}:
        if passage exists AND neighbour not yet visited:
            mark neighbour visited = 1
            record parent
            enqueue neighbour
```

Because BFS explores cells in order of increasing distance from the start, the first time the end cell is dequeued it was reached via the shortest path.

**5. Path reconstruction**

Walk backward from end to start using the parent arrays, marking each cell `2`:

```c
x = end_x; y = end_y;
while (!(x == 0 && y == 0)) {
    grid->visited[y][x] = 2;
    px = par_x[y][x];
    py = par_y[y][x];
    x = px; y = py;
}
grid->visited[0][0] = 2;  /* mark start */
```

**6. Cleanup**

Free the parent arrays and the queue. Return `0` (solved) or `-1` (no path).

---

## `grid->visited` Output Contract

After `solve_maze()` returns, the renderer reads `grid->visited` as follows:

| Value | Meaning | Rendered as |
|-------|---------|-------------|
| `0` | Not reached during BFS | `background_color` (open corridor) |
| `1` | Explored by BFS, not on the path | Blended tint (if `show_visited = 1`) |
| `2` | On the shortest solution path | `path_color` |

Wall cells are identified separately via `grid->cells` (a cell with no open passages in a given direction has a wall there); they are never written into `grid->visited`.

---

## Return Values

| Value | Meaning |
|-------|---------|
| `0`  | Solution found; `grid->visited` populated |
| `-1` | No path exists between start and end |

A perfect maze (produced by the Recursive Backtracker) always has exactly one path between any two cells, so `-1` should never occur in normal operation. The check is included for correctness.

---

## Complexity

| Metric | Value |
|--------|-------|
| Time   | O(W × H) — each cell is enqueued and dequeued at most once |
| Space  | O(W × H) — queue + two parent arrays |

where W = `grid->width` and H = `grid->height`.

---

## Integration with `main.c`

```c
generate_maze(&grid, config.seed);

if (solve_maze(&grid) == 0) {
    printf("Maze solved.\n");
} else {
    printf("No solution found.\n");
}

/* render_maze reads grid->cells and grid->visited */
```

The solver sits between the generator and the renderer in the pipeline:

```
Config → Parser → Grid alloc → generate_maze() → solve_maze() → render_maze()
```
