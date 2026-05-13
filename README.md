# Operating Systems Project

## Group Members

- Dana Salh
- Karin Marien

---

##  Description

This project implements a **directed weighted graph system** in C.

The project was developed as part of the Operating Systems course and includes:

- Reading a graph from an input file
- Building the graph dynamically in memory
- Finding the shortest path between two vertices using **Dijkstra’s Algorithm**
- Displaying the graph visually using **Raylib**

The graph supports:

- Directed edges
- Weighted edges
- Dynamic memory allocation
- Input validation
- Visual representation

---

##  Structure

```bash
operating-systems-project/
├── main.c
├── graph.h
├── graph.c
├── dijkstra.h
├── dijkstra.c
├── GUI/
│   ├── gui.h
│   └── gui.c
├── tests/
│   ├── test1.txt
│   ├── test2.txt
│   ├── test3.txt
│   └── test4.txt
├── Makefile
├── .gitignore
└── README.md
```

---

## Dependencies

This project requires:

### Linux / Ubuntu

Install required packages:

```bash
sudo apt install gcc make valgrind cmake
```

### Raylib

This project uses **Raylib** for graphical visualization.

Installation:

```bash
git clone https://github.com/raysan5/raylib.git
cd raylib
mkdir build
cd build
cmake ..
make
sudo make install
```

---

## Milestone 1

Implemented the core graph algorithms.

### Features

- Dynamic graph creation
- Adjacency matrix representation
- Reading graph from input file
- Dijkstra shortest path algorithm
- Path reconstruction
- Input validation
- Memory cleanup

### Tested Cases

- Valid shortest path
- Source equals destination
- No path found
- Invalid input

### Memory Validation

Memory was tested using:

```bash
valgrind --leak-check=full
```

Result:

```text
No memory leaks detected
```

---

## Milestone 2

Implemented graphical visualization using **Raylib**.

### Features

- Static graph visualization
- Automatic circular vertex placement
- Directed edges with arrow heads
- Edge weights display
- Vertex labels

### GUI Design

The graph is displayed using:

- Blue nodes
- Gray directed edges
- Red edge weights

---