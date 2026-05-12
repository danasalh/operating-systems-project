#ifndef GRAPH_H
#define GRAPH_H

#define INF 1000000000

typedef struct {
    int numVertices;
    int numEdges;
    int **matrix;
} Graph;

Graph* createGraph(int numVertices, int numEdges);
void addEdge(Graph* graph, int src, int dst, int weight);
void freeGraph(Graph* graph);

#endif