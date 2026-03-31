# 50.051-Programming-Language-Concepts

# Maze Generator, Solver & BMP Renderer (C)

## 📌 Overview

This project implements a complete pipeline for:

1. Parsing a configuration file (INI format)
2. Generating a maze using an iterative algorithm
3. Solving the maze using BFS
4. Rendering the result as a BMP image

The system is modular, with each component designed and implemented independently, then integrated through a shared data structure.

---

## 🧠 Architecture

```
config.ini → Parser → Config struct
                          ↓
                   Maze Generator
                          ↓
                     BFS Solver
                          ↓
                    BMP Renderer
                          ↓
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

- Implements iterative Recursive Backtracker algorithm
- Uses a custom stack for traversal
- Generates a valid maze inside the grid

---

### 🔹 BFS Solver (Matthew Phua)

- Solves the maze using Breadth-First Search
- Uses a custom queue
- Tracks visited cells and reconstructs the solution path

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
solver/bfs.c solver/queue.c \
render/bmp.c \
-o maze_solver
```

---

## ▶️ Usage

```bash
make run
```

---

## 📝 Example `config.ini`

```
[maze]
width=20
height=15
seed=42

[solver]
show_visited=1
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
4. Solve maze using BFS
5. Render output to BMP file

---

## ⚠️ Design Constraints

- Modular design: each component must be independent
- No global state shared across modules (except via `Grid`)
- Custom data structures required:
  - Stack (for generator)
  - Queue (for BFS)

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
