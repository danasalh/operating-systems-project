#include "dijkstra.h"

#include <stdio.h>
#include <stdlib.h>
#include "dijkstra.h"


static int findMinDistance(int* dist, int* visited, int n) {
    int min = INF;
    int minIndex = -1;

    for (int i = 0; i < n; i++) {
        if (!visited[i] && dist[i] < min) {
            min = dist[i];
            minIndex = i;
        }
    }

    return minIndex;
}


int dijkstra(Graph *graph, int source, int destination, int *path, int *pathLength){
     int n = graph->numVertices;

    int* dist = malloc(n * sizeof(int));
    int* visited = malloc(n * sizeof(int));
    int* prev = malloc(n * sizeof(int));

    if (dist == NULL || visited == NULL || prev == NULL) {
        printf("Memory allocation failed\n");
        free(dist);
        free(visited);
        free(prev);
        return INF;
    }

    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        visited[i] = 0;
        prev[i] = -1;
    }

    dist[source] = 0;

    for (int count = 0; count < n; count++) {
        int u = findMinDistance(dist, visited, n);

        if (u == -1) {
            break;
        }

        visited[u] = 1;

        for (int v = 0; v < n; v++) {
            if (!visited[v] &&
                graph->matrix[u][v] != INF &&
                dist[u] != INF &&
                dist[u] + graph->matrix[u][v] < dist[v]) {

                dist[v] = dist[u] + graph->matrix[u][v];
                prev[v] = u;
            }
        }
    }

    if (dist[destination] == INF) {
        *pathLength = 0;
        free(dist);
        free(visited);
        free(prev);
        return INF;
    }

    int temp[100];
    int len = 0;
    int current = destination;

    while (current != -1) {
        temp[len++] = current;
        current = prev[current];
    }

    *pathLength = len;

    for (int i = 0; i < len; i++) {
        path[i] = temp[len - 1 - i];
    }

    int result = dist[destination];

    free(dist);
    free(visited);
    free(prev);

    return result;
}