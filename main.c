#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "dijkstra.h"
#include "GUI/gui.h"

typedef struct {
    int source;
    int destination;
    int* path;
    int pathLength;
    int totalWeight;
} Traveler;

static Graph* readGraphFromFile(FILE* file) {
    int numVertices, numEdges;

    if (fscanf(file, "%d %d", &numVertices, &numEdges) != 2) {
        return NULL;
    }

    if (numVertices < 0 || numEdges < 0) {
        return NULL;
    }

    Graph* graph = createGraph(numVertices, numEdges);
    if (graph == NULL) {
        return NULL;
    }

    for (int i = 0; i < numEdges; i++) {
        int src, dst, weight;

        if (fscanf(file, "%d %d %d", &src, &dst, &weight) != 3) {
            freeGraph(graph);
            return NULL;
        }

        if (src < 0 || dst < 0 || weight < 0 ||
            src >= numVertices || dst >= numVertices) {
            freeGraph(graph);
            return NULL;
        }

        addEdge(graph, src, dst, weight);
    }

    return graph;
}

static Traveler* readTravelersFromFile(FILE* file, int numVertices, int* numTravelers) {
    if (fscanf(file, "%d", numTravelers) != 1) {
        return NULL;
    }

    if (*numTravelers <= 0) {
        return NULL;
    }

    Traveler* travelers = malloc(*numTravelers * sizeof(Traveler));
    if (travelers == NULL) {
        return NULL;
    }

    for (int i = 0; i < *numTravelers; i++) {
        int source, destination;

        if (fscanf(file, "%d %d", &source, &destination) != 2) {
            free(travelers);
            return NULL;
        }

        if (source < 0 || destination < 0 ||
            source >= numVertices || destination >= numVertices) {
            free(travelers);
            return NULL;
        }

        travelers[i].source = source;
        travelers[i].destination = destination;
        travelers[i].path = NULL;
        travelers[i].pathLength = 0;
        travelers[i].totalWeight = INF;
    }

    return travelers;
}

static int computeTravelerPaths(Graph* graph, Traveler* travelers, int numTravelers) {
    for (int i = 0; i < numTravelers; i++) {
        travelers[i].path = malloc(graph->numVertices * sizeof(int));
        if (travelers[i].path == NULL) {
            return 0;
        }

        travelers[i].totalWeight = dijkstra(
            graph,
            travelers[i].source,
            travelers[i].destination,
            travelers[i].path,
            &travelers[i].pathLength
        );
    }

    return 1;
}

static void printTravelerPaths(Traveler* travelers, int numTravelers) {
    for (int i = 0; i < numTravelers; i++) {
        printf("Traveler %d: %d -> %d\n",
               i,
               travelers[i].source,
               travelers[i].destination);

        if (travelers[i].totalWeight == INF) {
            printf("No path found\n");
            continue;
        }

        for (int j = 0; j < travelers[i].pathLength; j++) {
            printf("%d", travelers[i].path[j]);

            if (j < travelers[i].pathLength - 1) {
                printf(" -> ");
            }
        }

        printf("\n%d\n", travelers[i].totalWeight);
    }
}

static void freeTravelers(Traveler* travelers, int numTravelers) {
    if (travelers == NULL) {
        return;
    }

    for (int i = 0; i < numTravelers; i++) {
        free(travelers[i].path);
    }

    free(travelers);
}

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

    Graph* graph = readGraphFromFile(file);
    if (graph == NULL) {
        printf("Invalid input\n");
        fclose(file);
        return 1;
    }

    int numTravelers = 0;
    Traveler* travelers = readTravelersFromFile(file, graph->numVertices, &numTravelers);
    if (travelers == NULL) {
        printf("Invalid input\n");
        freeGraph(graph);
        fclose(file);
        return 1;
    }

    fclose(file);

    if (!computeTravelerPaths(graph, travelers, numTravelers)) {
        printf("Memory allocation failed\n");
        freeTravelers(travelers, numTravelers);
        freeGraph(graph);
        return 1;
    }

    printTravelerPaths(travelers, numTravelers);

    drawGraph(graph);

    freeTravelers(travelers, numTravelers);
    freeGraph(graph);

    return 0;
}
