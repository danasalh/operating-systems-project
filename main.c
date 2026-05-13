#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "dijkstra.h"
#include "GUI/gui.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    int numVertices, numEdges;

    if (fscanf(file, "%d %d", &numVertices, &numEdges) != 2) {
        printf("Invalid input\n");
        fclose(file);
        return 1;
    }

    if (numVertices < 0 || numEdges < 0) {
        printf("Invalid input\n");
        fclose(file);
        return 1;
    }

    Graph* graph = createGraph(numVertices, numEdges);
    if (graph == NULL) {
        fclose(file);
        return 1;
    }

    for (int i = 0; i < numEdges; i++) {
        int src, dst, weight;

        if (fscanf(file, "%d %d %d", &src, &dst, &weight) != 3) {
            printf("Invalid input\n");
            freeGraph(graph);
            fclose(file);
            return 1;
        }

        if (src < 0 || dst < 0 || weight < 0 ||
            src >= numVertices || dst >= numVertices) {
            printf("Invalid input\n");
            freeGraph(graph);
            fclose(file);
            return 1;
        }

        addEdge(graph, src, dst, weight);
    }

    int source, destination;

    if (fscanf(file, "%d %d", &source, &destination) != 2) {
        printf("Invalid input\n");
        freeGraph(graph);
        fclose(file);
        return 1;
    }

    fclose(file);

    if (source < 0 || destination < 0 ||
        source >= numVertices || destination >= numVertices) {
        printf("Invalid input\n");
        freeGraph(graph);
        return 1;
    }

    int* path = malloc(numVertices * sizeof(int));
    if (path == NULL) {
        printf("Memory allocation failed\n");
        freeGraph(graph);
        return 1;
    }

    int pathLength = 0;
    int totalWeight = dijkstra(graph, source, destination, path, &pathLength);

    if (totalWeight == INF) {
        printf("No path found\n");
    } else {
        for (int i = 0; i < pathLength; i++) {
            printf("%d", path[i]);

            if (i < pathLength - 1) {
                printf(" -> ");
            }
        }

        printf("\n%d\n", totalWeight);
    }

    drawGraph(graph);


    free(path);
    freeGraph(graph);

    return 0;
}