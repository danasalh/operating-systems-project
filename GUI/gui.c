#include "gui.h"
#include "../ipc.h"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 700
#define NODE_RADIUS 25

static Color travelerColors[] = {
    RED, GREEN, BLUE, ORANGE, PURPLE, MAROON, DARKGREEN, DARKBLUE
};

typedef struct {
    int occupiedBy;
    int* queue;
    int queueSize;
} NodeQueue;

static void calculatePositions(Vector2 positions[], int n) {
    Vector2 center = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    float radius = 250.0f;

    for (int i = 0; i < n; i++) {
        float angle = (2.0f * PI * i) / n;
        positions[i].x = center.x + radius * cosf(angle);
        positions[i].y = center.y + radius * sinf(angle);
    }
}

static void drawEdges(Graph* graph, Vector2 positions[], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {

            if (i != j && graph->matrix[i][j] != INF) {

                DrawLineEx(positions[i], positions[j], 2.0f, GRAY);

                float angle = atan2f(
                    positions[j].y - positions[i].y,
                    positions[j].x - positions[i].x
                );

                float arrowSize = 16.0f;

                Vector2 arrowTip = {
                    positions[j].x - NODE_RADIUS * cosf(angle),
                    positions[j].y - NODE_RADIUS * sinf(angle)
                };

                Vector2 arrowLeft = {
                    arrowTip.x - arrowSize * cosf(angle - PI / 6),
                    arrowTip.y - arrowSize * sinf(angle - PI / 6)
                };

                Vector2 arrowRight = {
                    arrowTip.x - arrowSize * cosf(angle + PI / 6),
                    arrowTip.y - arrowSize * sinf(angle + PI / 6)
                };

                DrawLineEx(arrowTip, arrowLeft, 2.0f, DARKGRAY);
                DrawLineEx(arrowTip, arrowRight, 2.0f, DARKGRAY);

                Vector2 mid = {
                    (positions[i].x + positions[j].x) / 2.0f,
                    (positions[i].y + positions[j].y) / 2.0f
                };

                char weightText[20];
                snprintf(weightText,
                         sizeof(weightText),
                         "%d",
                         graph->matrix[i][j]);

                DrawText(weightText,
                         (int)mid.x,
                         (int)mid.y,
                         18,
                         RED);
            }
        }
    }
}

static void drawNodes(Vector2 positions[], int n) {
    for (int i = 0; i < n; i++) {
        DrawCircleV(positions[i], NODE_RADIUS, SKYBLUE);
        DrawCircleLines(positions[i].x, positions[i].y, NODE_RADIUS, DARKBLUE);

        char label[20];
        snprintf(label, sizeof(label), "%d", i);
        DrawText(label, positions[i].x - 5, positions[i].y - 10, 20, BLACK);
    }
}

static const char* schedulerName(SchedulerAlgorithm scheduler) {
    if (scheduler == SCHEDULER_SJF) {
        return "SJF";
    }

    return "FCFS";
}

static int selectNextTraveler(NodeQueue* queue,
                              Traveler* travelers,
                              SchedulerAlgorithm scheduler) {
    int selectedIndex = 0;

    if (scheduler == SCHEDULER_SJF) {
        for (int i = 1; i < queue->queueSize; i++) {
            int current = queue->queue[i];
            int selected = queue->queue[selectedIndex];

            if (travelers[current].burst < travelers[selected].burst ||
                (travelers[current].burst == travelers[selected].burst &&
                 travelers[current].arrival < travelers[selected].arrival)) {
                selectedIndex = i;
            }
        }
    }

    int travelerIndex = queue->queue[selectedIndex];

    for (int i = selectedIndex; i < queue->queueSize - 1; i++) {
        queue->queue[i] = queue->queue[i + 1];
    }

    queue->queueSize--;
    return travelerIndex;
}

static void grantNode(NodeQueue* nodeQueues,
                      Traveler* travelers,
                      int travelerIndex,
                      int node) {
    TravelerCommand command;

    command.granted = 1;
    command.node = node;

    nodeQueues[node].occupiedBy = travelerIndex;
    travelers[travelerIndex].waiting = 0;

    write(travelers[travelerIndex].controlFd[1], &command, sizeof(command));
}

static void scheduleNode(NodeQueue* nodeQueues,
                         Traveler* travelers,
                         int node,
                         SchedulerAlgorithm scheduler) {
    if (nodeQueues[node].occupiedBy != -1 || nodeQueues[node].queueSize == 0) {
        return;
    }

    int travelerIndex = selectNextTraveler(&nodeQueues[node], travelers, scheduler);
    grantNode(nodeQueues, travelers, travelerIndex, node);
}

static void enqueueTraveler(NodeQueue* nodeQueues,
                            Traveler* travelers,
                            int travelerIndex,
                            int node,
                            int* arrivalCounter) {
    NodeQueue* queue = &nodeQueues[node];

    travelers[travelerIndex].waiting = 1;
    travelers[travelerIndex].requestedNode = node;
    travelers[travelerIndex].arrival = (*arrivalCounter)++;

    queue->queue[queue->queueSize] = travelerIndex;
    queue->queueSize++;
}

static void handleNodeRequest(NodeQueue* nodeQueues,
                              Traveler* travelers,
                              TravelerMessage* msg,
                              int* arrivalCounter,
                              SchedulerAlgorithm scheduler) {
    int travelerIndex = msg->travelerIndex;
    int node = msg->currentNode;

    travelers[travelerIndex].currentNode = node;
    travelers[travelerIndex].nextNode = msg->nextNode;
    travelers[travelerIndex].finished = msg->finished;

    enqueueTraveler(nodeQueues, travelers, travelerIndex, node, arrivalCounter);
    scheduleNode(nodeQueues, travelers, node, scheduler);
}

static void handleArrival(Traveler* travelers, TravelerMessage* msg) {
    int travelerIndex = msg->travelerIndex;

    travelers[travelerIndex].currentNode = msg->currentNode;
    travelers[travelerIndex].nextNode = msg->nextNode;
    travelers[travelerIndex].finished = msg->finished;
    travelers[travelerIndex].waiting = 0;

    if (msg->finished) {
        printf("[PID=%ld] arrived at node %d | DESTINATION\n",
               (long)msg->pid,
               msg->currentNode);
    }
    else {
        printf("[PID=%ld] arrived at node %d | next node: %d\n",
               (long)msg->pid,
               msg->currentNode,
               msg->nextNode);
    }

    fflush(stdout);
}

static void handleLeave(NodeQueue* nodeQueues,
                        Traveler* travelers,
                        TravelerMessage* msg,
                        SchedulerAlgorithm scheduler) {
    int node = msg->currentNode;

    if (nodeQueues[node].occupiedBy == msg->travelerIndex) {
        nodeQueues[node].occupiedBy = -1;
    }

    scheduleNode(nodeQueues, travelers, node, scheduler);
}

static void handleFinished(Traveler* travelers, TravelerMessage* msg) {
    int travelerIndex = msg->travelerIndex;

    travelers[travelerIndex].finished = 1;
    travelers[travelerIndex].waiting = 0;
    travelers[travelerIndex].completed = 1;

    printf("[PID=%ld] finished\n", (long)msg->pid);
    fflush(stdout);
}

static void readTravelerMessages(Traveler* travelers,
                                 int numTravelers,
                                 NodeQueue* nodeQueues,
                                 int* arrivalCounter,
                                 SchedulerAlgorithm scheduler) {
    for (int i = 0; i < numTravelers; i++) {
        TravelerMessage msg;

        while (read(travelers[i].pipeFd[0], &msg, sizeof(TravelerMessage)) > 0) {
            switch (msg.type) {
                case MSG_REQUEST_NODE:
                    handleNodeRequest(nodeQueues,
                                      travelers,
                                      &msg,
                                      arrivalCounter,
                                      scheduler);
                    break;

                case MSG_ARRIVED_NODE:
                    handleArrival(travelers, &msg);
                    break;

                case MSG_LEFT_NODE:
                    handleLeave(nodeQueues, travelers, &msg, scheduler);
                    break;

                case MSG_FINISHED:
                    handleFinished(travelers, &msg);
                    break;
            }
        }
    }
}

static void drawTravelers(Traveler* travelers, int numTravelers, Vector2 positions[]) {
    for (int i = 0; i < numTravelers; i++) {
        int node = travelers[i].currentNode;

        if (node < 0) {
            continue;
        }

        Color color;

        if (travelers[i].waiting) {
            color = YELLOW;
        } else {
            color = travelerColors[i % 8];
        }

        Vector2 pos = positions[node];

        DrawCircleV(pos, 13, color);

        char text[20];
        snprintf(text, sizeof(text), "T%d", i);
        DrawText(text, pos.x - 10, pos.y - 10, 16, WHITE);
    }
}

static int allTravelersCompleted(Traveler* travelers, int numTravelers) {
    for (int i = 0; i < numTravelers; i++) {
        if (!travelers[i].completed) {
            return 0;
        }
    }

    return 1;
}

static NodeQueue* createNodeQueues(int numNodes, int numTravelers) {
    NodeQueue* nodeQueues = malloc(numNodes * sizeof(NodeQueue));
    if (nodeQueues == NULL) {
        return NULL;
    }

    for (int i = 0; i < numNodes; i++) {
        nodeQueues[i].occupiedBy = -1;
        nodeQueues[i].queueSize = 0;
        nodeQueues[i].queue = malloc(numTravelers * sizeof(int));

        if (nodeQueues[i].queue == NULL) {
            for (int j = 0; j < i; j++) {
                free(nodeQueues[j].queue);
            }

            free(nodeQueues);
            return NULL;
        }
    }

    return nodeQueues;
}

static void freeNodeQueues(NodeQueue* nodeQueues, int numNodes) {
    if (nodeQueues == NULL) {
        return;
    }

    for (int i = 0; i < numNodes; i++) {
        free(nodeQueues[i].queue);
    }

    free(nodeQueues);
}

void drawGraph(Graph* graph,
               Traveler* travelers,
               int numTravelers,
               SchedulerAlgorithm scheduler) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,  "Milestone 7 - Scheduling");
    SetTargetFPS(60);

    int n = graph->numVertices;
    Vector2* positions = malloc(n * sizeof(Vector2));
    NodeQueue* nodeQueues = createNodeQueues(n, numTravelers);
    int arrivalCounter = 0;

    if (positions == NULL || nodeQueues == NULL) {
        free(positions);
        freeNodeQueues(nodeQueues, n);
        CloseWindow();
        return;
    }

    calculatePositions(positions, n);

    while (!WindowShouldClose() && !allTravelersCompleted(travelers, numTravelers)) {
        readTravelerMessages(travelers,
                             numTravelers,
                             nodeQueues,
                             &arrivalCounter,
                             scheduler);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Directed Weighted Graph - Milestone 7", 20, 20, 24, DARKBLUE);
        DrawText(schedulerName(scheduler), 20, 50, 22, MAROON);

        drawEdges(graph, positions, n);
        drawNodes(positions, n);
        drawTravelers(travelers, numTravelers, positions);

        EndDrawing();
    }

    CloseWindow();

    while (!allTravelersCompleted(travelers, numTravelers)) {
        readTravelerMessages(travelers,
                             numTravelers,
                             nodeQueues,
                             &arrivalCounter,
                             scheduler);
        sleep(1);
    }

    freeNodeQueues(nodeQueues, n);
    free(positions);
}
