#include <stdio.h>
#include <stdlib.h>
#include "graph.h"


Graph* createGraph(int numVertices, int numEdges) {
      Graph* graph = malloc(sizeof(Graph));
    if (graph == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    graph->numVertices = numVertices;
    graph->numEdges = numEdges;

    graph->matrix = malloc(numVertices * sizeof(int*));
    if (graph->matrix == NULL) {
        printf("Memory allocation failed\n");
        free(graph);
        return NULL;
    }

    for (int i = 0; i < numVertices; i++) {
        graph->matrix[i] = malloc(numVertices * sizeof(int));
        if (graph->matrix[i] == NULL) {
            printf("Memory allocation failed\n");

            for (int k = 0; k < i; k++) {
                free(graph->matrix[k]);
            }

            free(graph->matrix);
            free(graph);
            return NULL;
        }

        for (int j = 0; j < numVertices; j++) {
            if (i == j) {
                graph->matrix[i][j] = 0;
            } else {
                graph->matrix[i][j] = INF;
            }
        }
    }

    return graph;


}


void addEdge(Graph* graph, int src, int dst, int weight) {
    graph->matrix[src][dst] = weight;
}



void freeGraph(Graph* graph) {
    if (graph == NULL) {
        return;
    }

    for (int i = 0; i < graph->numVertices; i++) {
        free(graph->matrix[i]);
    }

    free(graph->matrix);
    free(graph);
}
       



    