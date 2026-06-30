#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#include "graph.h"
#include "dijkstra.h"
#include "traveler.h"
#include "ipc.h"
#include "GUI/gui.h"

static Graph* readGraphFromFile(FILE* file) {
    int numVertices, numEdges;

    if (fscanf(file, "%d %d", &numVertices, &numEdges) != 2) {
        return NULL;
    }

    if (numVertices <= 0 || numEdges < 0) {
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

        travelers[i].pid = -1;

        travelers[i].pipeFd[0] = -1;
        travelers[i].pipeFd[1] = -1;
        travelers[i].grantFd[0] = -1;
        travelers[i].grantFd[1] = -1;

        travelers[i].currentNode = source;
        travelers[i].nextNode = destination;
        travelers[i].finished = 0;
        travelers[i].waiting = 0;

        travelers[i].arrival = 0;
        travelers[i].burst = 0;
        travelers[i].turnaround = 0;
    }

    return travelers;
}

static int remainingCost(Graph* graph, int* path, int pathLength, int index) {
    int cost = 0;

    for (int i = index; i < pathLength - 1; i++) {
        int from = path[i];
        int to = path[i + 1];

        if (graph->matrix[from][to] != INF) {
            cost += graph->matrix[from][to];
        }
    }

    return cost;
}

static void sendMessage(
    int fd,
    int index,
    int current,
    int next,
    int finished,
    int waiting,
    int remaining,
    int type
) {
    TravelerMessage msg;

    msg.pid = getpid();
    msg.travelerIndex = index;
    msg.currentNode = current;
    msg.nextNode = next;
    msg.finished = finished;
    msg.waiting = waiting;
    msg.remainingCost = remaining;
    msg.type = type;

    write(fd, &msg, sizeof(msg));
}

static void childProcess(Graph* graph, Traveler traveler, int index, int writeFd, int grantReadFd) {
    int* path = malloc(graph->numVertices * sizeof(int));
    if (path == NULL) {
        close(writeFd);
        close(grantReadFd);
        exit(1);
    }

    int pathLength = 0;
    int totalWeight = dijkstra(
        graph,
        traveler.source,
        traveler.destination,
        path,
        &pathLength
    );

    if (totalWeight == INF || pathLength == 0) {
        sendMessage(writeFd, index, traveler.source, -1, 1, 0, 0, MSG_ENTERED);
        free(path);
        close(writeFd);
        close(grantReadFd);
        exit(0);
    }

    for (int i = 0; i < pathLength; i++) {
        int current = path[i];
        int next = (i < pathLength - 1) ? path[i + 1] : -1;
        int finished = (i == pathLength - 1);
        int remaining = remainingCost(graph, path, pathLength, i);

        sendMessage(writeFd, index, current, next, finished, 1, remaining, MSG_REQUEST);

        char grant;
        if (read(grantReadFd, &grant, 1) <= 0) {
            break;
        }

        sendMessage(writeFd, index, current, next, finished, 0, remaining, MSG_ENTERED);

        sleep(1);

        sendMessage(writeFd, index, current, next, finished, 0, remaining, MSG_LEFT);
    }

    free(path);
    close(writeFd);
    close(grantReadFd);
    exit(0);
}

static int createChildProcesses(Graph* graph, Traveler* travelers, int numTravelers) {
    for (int i = 0; i < numTravelers; i++) {
        if (pipe(travelers[i].pipeFd) == -1) {
            perror("pipe failed");
            return 0;
        }

        if (pipe(travelers[i].grantFd) == -1) {
            perror("grant pipe failed");
            return 0;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            return 0;
        }

        if (pid == 0) {
            close(travelers[i].pipeFd[0]);
            close(travelers[i].grantFd[1]);

            childProcess(
                graph,
                travelers[i],
                i,
                travelers[i].pipeFd[1],
                travelers[i].grantFd[0]
            );
        }

        travelers[i].pid = pid;

        close(travelers[i].pipeFd[1]);
        close(travelers[i].grantFd[0]);

        int flags = fcntl(travelers[i].pipeFd[0], F_GETFL, 0);
        fcntl(travelers[i].pipeFd[0], F_SETFL, flags | O_NONBLOCK);
    }

    return 1;
}

static void waitForChildren(Traveler* travelers, int numTravelers) {
    for (int i = 0; i < numTravelers; i++) {
        if (travelers[i].pid > 0) {
            waitpid(travelers[i].pid, NULL, 0);
        }
    }
}

static void freeTravelers(Traveler* travelers, int numTravelers) {
    if (travelers == NULL) {
        return;
    }

    for (int i = 0; i < numTravelers; i++) {
        free(travelers[i].path);

        if (travelers[i].pipeFd[0] != -1) {
            close(travelers[i].pipeFd[0]);
        }

        if (travelers[i].grantFd[1] != -1) {
            close(travelers[i].grantFd[1]);
        }
    }

    free(travelers);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s -schd fcfs <input_file>\n", argv[0]);
        printf("Usage: %s -schd sjf <input_file>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-schd") != 0) {
        printf("Invalid option. Use -schd\n");
        return 1;
    }

    SchedulerType scheduler;

    if (strcmp(argv[2], "fcfs") == 0) {
        scheduler = SCHED_FCFS;
    }
    else if (strcmp(argv[2], "sjf") == 0) {
        scheduler = SCHED_SJF;
    }
    else {
        printf("Unknown scheduler: %s\n", argv[2]);
        return 1;
    }

    FILE* file = fopen(argv[3], "r");
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

    if (!createChildProcesses(graph, travelers, numTravelers)) {
        freeTravelers(travelers, numTravelers);
        freeGraph(graph);
        return 1;
    }

    drawGraph(graph, travelers, numTravelers, scheduler);

    waitForChildren(travelers, numTravelers);

    freeTravelers(travelers, numTravelers);
    freeGraph(graph);

    return 0;
}