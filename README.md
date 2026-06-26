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
│   ├── test4.txt
│   └── processes_test.txt
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



## Milestone 5

### Implementation

In this milestone, inter-process communication (IPC) was implemented using pipes.

Each traveler is represented by a child process created using `fork()`. After creation, every child independently computes its shortest path using the Dijkstra algorithm.

A dedicated pipe is created between the parent process and each child process. The child sends progress updates to the parent while traversing its route. These updates include the current node, the next node, and a completion flag indicating whether the destination has been reached.

The GUI displays all travelers simultaneously on the directed weighted graph while the IPC communication is running in the background.

### Features

* Multiple traveler processes using `fork()`
* IPC communication using pipes
* Independent Dijkstra computation by each child process
* Real-time status messages from child processes to the parent
* PID-based logging
* Directed weighted graph visualization
* Multiple travelers displayed simultaneously

### Build & Run

```bash
make milestone5
./sim tests/test7.txt
```

### Example Output

```text
[PID=503645] arrived at node 0 | next node: 2
[PID=503646] arrived at node 1 | next node: 3
[PID=503647] arrived at node 2 | next node: 1
[PID=503645] arrived at node 5 | DESTINATION
[PID=503645] finished
```


## Milestone 6

### Synchronization of Node Access

In this milestone, synchronization was added to ensure that no more than one traveler can occupy a node at the same time.

#### Synchronization Mechanism

* A semaphore is associated with each graph node.
* Before entering a node, a traveler must acquire the node semaphore.
* If the node is occupied, the traveler waits until the semaphore becomes available.
* After staying in the node for one second, the traveler releases the semaphore and continues.

#### IPC Mechanism

* Communication between child processes and the parent process is implemented using pipes.
* Each traveler process sends status updates to the parent.
* The parent receives messages and updates the GUI and console logs.

#### GUI Features

* Travelers currently inside a node are displayed normally.
* Travelers waiting for a busy node are displayed in a different color.
* Console messages indicate when a traveler is waiting, enters a node, reaches the destination, or finishes execution.

### Build and Run

```bash
make milestone6
./sim tests/test8.txt
```




## Milestone 7

### CPU Scheduling Algorithms

In this milestone, the simulator was extended to support multiple CPU scheduling algorithms for traveler execution.

The scheduling algorithm is selected through the command-line argument when running the program.

Supported algorithms:

- FCFS (First Come First Served)
- SJF (Shortest Job First)

The scheduler determines which traveler is allowed to execute according to the selected scheduling policy while 
preserving the synchronization mechanisms implemented in previous milestones.

### FCFS

FCFS executes travelers according to their arrival order.

Characteristics:

- Simple queue implementation
- Non-preemptive scheduling
- Fair according to arrival time
- May result in long waiting times if a long process arrives first

### SJF

SJF always selects the traveler with the shortest execution time (burst time).

Characteristics:

- Non-preemptive scheduling
- Reduces average waiting time
- Gives priority to shorter processes
- Requires knowledge (or estimation) of burst times

### Comparison

| Feature | FCFS | SJF |
|--------|------|------|
| Scheduling policy | Arrival order | Shortest burst first |
| Preemptive | No | No |
| Average waiting time | Higher | Lower |
| Fairness | High | Lower for long jobs |
| Starvation | No | Possible for long processes |

Compile:

```bash
make milestone7
```

Run FCFS:

```bash
./sim-schd fcfs <input_file>
```

Example:

```bash
./sim-schd fcfs tests/test9.txt
```

Run SJF:

```bash
./sim-schd sjf <input_file>
```

### Input Format

The input file contains the list of processes together with their scheduling parameters.

Each process includes:

- Process ID
- Arrival time
- Burst time

The scheduler reads all processes from the input file and simulates their execution according to the selected 
scheduling algorithm.

### Output

During execution, the simulator prints:

- Current simulation time
- Process arrival
- Process selection by the scheduler
- Process execution
- Process completion
- Waiting time
- Turnaround time

At the end of the simulation, a summary is displayed including the average waiting time and average turnaround 
time for the selected scheduling algorithm.

