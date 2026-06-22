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

### Linux 

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


## Milestone 4

Implemented support for multiple travelers using operating system processes.

### Features

- Support for multiple travelers in a single input file
- Added `Traveler` structure to store traveler information
- Shortest path computation for each traveler using Dijkstra's algorithm
- Created a separate process for each traveler using `fork()`
- Parent process manages all travelers
- Child processes are created and identified by their PID
- Graphical visualization of multiple travelers
- Each traveler is displayed in a different color
- Travelers move along their shortest path in the graph
- Memory management and cleanup for all travelers and processes

### Input Format

Example:

```text
6 8
0 1 4
0 2 2
1 3 5
2 1 1
2 3 8
3 4 2
4 5 3
2 5 10

3
0 5
1 4
2 3
```

Where:

- First line: number of vertices and edges
- Following lines: graph edges and weights
- Last section:
  - number of travelers
  - source and destination for each traveler

### Build and Run

```bash
make milestone4
./sim tests/test7.txt
```

### GUI Features

- Directed weighted graph visualization
- Automatic node placement
- Edge weight display
- Multiple travelers displayed simultaneously
- Different color for each traveler
- Traveler movement animation along shortest paths


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