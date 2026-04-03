# 50.051-Programming-Language-Concepts

# Maze Generator, Solver & BMP Renderer (C)

## 📌 Overview

This project is a high-performance, modular maze pipeline built entirely in ANSI C. It translates user-defined configurations into complex geometric structures, solves them using various pathfinding strategies, and renders the result as a bit-mapped image.

It implements a complete pipeline for:

1. Parsing a configuration file (INI format) or CLI input
2. Generating a maze using Prim's algorithm or Recursive Backtracker
3. Solving the maze using BFS/DFS
4. Rendering the result as a BMP image

The system is modular, with each component designed and implemented independently, then integrated through a shared data structure.

---

## 🧠 Architecture

```
[config.ini] ──┐
               ├─► [FSM Parser] ──┐
[CLI Args] ────┘                  │
                                  ▼
                          [Config Struct]
                                  │
          ┌───────────────────────┴───────────────────────┐
          ▼                                               ▼
 [Generator Factory]                             [Solver Factory]
 (Recursive Backtracker)                                (BFS)
 (Prim's Algorithm)                                     (DFS)
          │                                               │
          └───────────────────────┬───────────────────────┘
                                  ▼
                          [BMP Render Engine]
                                  │
                                  ▼
                             output.bmp
```

---

## 📁 Project Structure

```
project/
│
├── main.c
├── Makefile
├── README.md
│
├── config/
│   ├── parser.c
│   ├── parser.h
│
├── maze/
│   ├── generator.c
│   ├── generator.h
│   ├── stack.c
│   ├── stack.h
│
├── solver/
│   ├── bfs.c
│   ├── bfs.h
│   ├── dfs.c
│   ├── dfs.h
│   ├── queue.c
│   ├── queue.h
│
├── render/
│   ├── bmp.c
│   ├── bmp.h
│
├── grid.h
```

---

## 🧩 Module Responsibilities

### 🔹 Config Parser (Liang Kuan Hon)

- Parses `config.ini` using a finite state machine (FSM)
- Extracts parameters such as:
  - maze dimensions
  - random seed
  - solver/render options

- Outputs a `Config` struct used by all modules

---

### 🔹 Maze Generator (Magnus Yunus)

- Implements iterative Recursive Backtracker and Prim's Algorithm
- Uses a custom stack for traversal
- Generates a valid maze inside the grid

---

### 🔹 BFS & DFS Solver (Matthew Phua)

- Solves the maze using Breadth-First Search (BFS) or Depth-First Search (DFS)
- BFS uses a custom queue; DFS reuses the existing stack from the maze generator
- Tracks visited cells and reconstructs the solution path
- Writes solution to `maze_solved.txt`

---

### 🔹 Renderer & Integration (Desmond Ngui)

- Writes maze and solution to a BMP file
- Handles:
  - BMP/DIB headers
  - Row padding (4-byte alignment)
  - Pixel encoding

- Integrates all modules in `main.c`

---

## ⚙️ Build Instructions

### Compile

```bash
make
```

Or manually:

```bash
gcc -Wall -Wextra -std=c11 main.c \
config/parser.c \
maze/generator.c maze/stack.c \
solver/bfs.c solver/queue.c solver/dfs.c \
render/bmp.c \
-o maze_solver
```

---

## ▶️ Usage

### 1. Basic execution

Run the engine using values defined in `config.ini`.

```bash
make run
```

---

### 2. Advanced overrides via CLI

CLI flags are parsed _after_ `config.ini` and take precedence in the final config struct.

**Override width only**

```bash
make run ARGS="--width 41"
```

**Full suite override**

```bash
make run ARGS="--width 41 --height 41 --seed 12345 --algo prim --solver dfs"
```

---

### 3. Direct binary execution

Invoke the binary manually, bypassing the `make` wrapper — useful for quick tests.

```bash
./maze_solver config.ini --width 25 --height 25 --algo prim
```

---

## 📝 Example `config.ini`

```
[maze]
width=20
height=15
seed=42
algorithm=prim    ; Options: backtracker, prim

[solver]
show_visited=1    ; Highlight pathfinding progress
algorithm=bfs     ; Options: bfs, dfs
```

---

## 🧱 Core Data Structure

All modules interact through a shared `Grid`:

```c
typedef struct {
    int width;
    int height;
    int **cells;
    int **visited;
} Grid;
```

---

## 🔁 Execution Flow

1. Parse configuration file
2. Initialize grid memory
3. Generate maze
4. Solve maze using BFS or DFS (selected via config)
5. Render output to BMP file

---

## ⚠️ Design Constraints

- Modular design: each component must be independent
- No global state shared across modules (except via `Grid`)
- Custom data structures required:
  - Stack (for generator and DFS solver)
  - Queue (for BFS solver)

- No external libraries beyond standard C

---

## 🧪 Testing

Recommended test cases:

- Small maze (e.g., 5x5)
- Large maze (e.g., 100x100)
- Invalid config formats
- Edge cases (1x1 maze, no solution)

---

## 📌 Key Challenges

- Correct FSM implementation for parsing
- Managing dynamic 2D memory safely
- Ensuring BFS correctness and path reconstruction
- Handling BMP binary format and padding accurately
- Maintaining clean module interfaces

---

## 👥 Team Contributions

- **Liang Kuan Hon** — Config Parser & FSM
- **Magnus Yunus** — Maze Generator & Stack
- **Matthew Phua** — BFS Solver & Queue
- **Desmond Ngui** — Renderer & Integration

---

## 🚀 Future Improvements (should we consider this?)

- Support for multiple output formats (PNG, ASCII)
- Interactive visualization
- Additional maze algorithms
- Weighted/pathfinding variations (A\*)

---

## 📄 License

This project is for SUTD 50.051 Progamming Language Concepts Module.
