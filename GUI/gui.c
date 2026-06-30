#include "gui.h"
#include "../ipc.h"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <unistd.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 700
#define NODE_RADIUS 25
#define MAX_NODES 100
#define MAX_QUEUE 100

typedef struct {
    int travelerIndex;
    pid_t pid;
    int node;
    int nextNode;
    int finished;
    int remainingCost;
    int order;
} WaitRequest;

static Color travelerColors[] = {
    RED, GREEN, BLUE, ORANGE, PURPLE, MAROON, DARKGREEN, DARKBLUE
};

static WaitRequest waitQueues[MAX_NODES][MAX_QUEUE];
static int queueSizes[MAX_NODES];
static int nodeOccupied[MAX_NODES];
static int arrivalOrder = 0;

static const char* schedulerName(SchedulerType scheduler) {
    if (scheduler == SCHED_FCFS) {
        return "FCFS";
    }

    return "SJF";
}

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
                snprintf(weightText, sizeof(weightText), "%d", graph->matrix[i][j]);
                DrawText(weightText, (int)mid.x, (int)mid.y, 18, RED);
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

static void enqueueRequest(WaitRequest request) {
    int node = request.node;

    if (node < 0 || node >= MAX_NODES) {
        return;
    }

    if (queueSizes[node] >= MAX_QUEUE) {
        return;
    }

    waitQueues[node][queueSizes[node]] = request;
    queueSizes[node]++;
}

static int chooseRequestIndex(int node, SchedulerType scheduler) {
    if (queueSizes[node] == 0) {
        return -1;
    }

    int selected = 0;

    if (scheduler == SCHED_FCFS) {
        int bestOrder = waitQueues[node][0].order;

        for (int i = 1; i < queueSizes[node]; i++) {
            if (waitQueues[node][i].order < bestOrder) {
                bestOrder = waitQueues[node][i].order;
                selected = i;
            }
        }
    }
    else {
        int bestCost = waitQueues[node][0].remainingCost;

        for (int i = 1; i < queueSizes[node]; i++) {
            if (waitQueues[node][i].remainingCost < bestCost) {
                bestCost = waitQueues[node][i].remainingCost;
                selected = i;
            }
        }
    }

    return selected;
}

static WaitRequest removeRequest(int node, int index) {
    WaitRequest selected = waitQueues[node][index];

    for (int i = index; i < queueSizes[node] - 1; i++) {
        waitQueues[node][i] = waitQueues[node][i + 1];
    }

    queueSizes[node]--;

    return selected;
}

static void grantNextTraveler(int node, Traveler* travelers, SchedulerType scheduler) {
    if (node < 0 || node >= MAX_NODES) {
        return;
    }

    if (nodeOccupied[node]) {
        return;
    }

    int index = chooseRequestIndex(node, scheduler);
    if (index == -1) {
        return;
    }

    WaitRequest selected = removeRequest(node, index);

    nodeOccupied[node] = 1;

    char grant = 'G';
    write(travelers[selected.travelerIndex].grantFd[1], &grant, 1);
}



static void readTravelerMessages(Traveler* travelers, int numTravelers, SchedulerType scheduler) {
    for (int i = 0; i < numTravelers; i++) {
        TravelerMessage msg;

        while (read(travelers[i].pipeFd[0], &msg, sizeof(TravelerMessage)) > 0) {

            if (msg.type == MSG_REQUEST) {
                travelers[i].currentNode = msg.currentNode;
                travelers[i].nextNode = msg.nextNode;
                travelers[i].waiting = 1;
                travelers[i].finished = msg.finished;

                WaitRequest request;
                request.travelerIndex = msg.travelerIndex;
                request.pid = msg.pid;
                request.node = msg.currentNode;
                request.nextNode = msg.nextNode;
                request.finished = msg.finished;
                request.remainingCost = msg.remainingCost;
                request.order = arrivalOrder++;

                enqueueRequest(request);

                printf("[PID=%d] waiting for node %d | remaining cost: %d\n",
                       msg.pid,
                       msg.currentNode,
                       msg.remainingCost);
            }

            else if (msg.type == MSG_ENTERED) {
                travelers[i].currentNode = msg.currentNode;
                travelers[i].nextNode = msg.nextNode;
                travelers[i].waiting = 0;
                travelers[i].finished = msg.finished;

                if (msg.finished) {
                    printf("[PID=%d] arrived at node %d | DESTINATION\n",
                           msg.pid,
                           msg.currentNode);
                } else {
                    printf("[PID=%d] arrived at node %d | next node: %d\n",
                           msg.pid,
                           msg.currentNode,
                           msg.nextNode);
                }
            }

            else if (msg.type == MSG_LEFT) {
                nodeOccupied[msg.currentNode] = 0;

                if (msg.finished) {
                    travelers[i].finished = 1;
                    printf("[PID=%d] finished\n", msg.pid);
                }
            }

            fflush(stdout);
        }
    }

    for (int node = 0; node < MAX_NODES; node++) {
        grantNextTraveler(node, travelers, scheduler);
    }
}



static void drawTravelers(Traveler* travelers, int numTravelers, Vector2 positions[]) {
    for (int i = 0; i < numTravelers; i++) {
        int node = travelers[i].currentNode;

        if (node < 0) {
            continue;
        }

        Vector2 pos = positions[node];

        if (travelers[i].waiting) {
            pos.y -= 40;
        }

        Color color;

        if (travelers[i].waiting) {
            color = YELLOW;
        }
        else {
            color = travelerColors[i % 8];
        }

        DrawCircleV(pos, 13, color);

        char text[20];
        snprintf(text, sizeof(text), "T%d", i);
        DrawText(text, pos.x - 10, pos.y - 10, 16, WHITE);
    }
}

static int allFinished(Traveler* travelers, int numTravelers) {
    for (int i = 0; i < numTravelers; i++) {
        if (!travelers[i].finished) {
            return 0;
        }
    }

    return 1;
}

void drawGraph(Graph* graph, Traveler* travelers, int numTravelers, SchedulerType scheduler) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Milestone 7 - Scheduling");
    SetTargetFPS(60);

    int n = graph->numVertices;
    Vector2 positions[15];

    for (int i = 0; i < MAX_NODES; i++) {
        queueSizes[i] = 0;
        nodeOccupied[i] = 0;
    }

    calculatePositions(positions, n);

    double finishTime = -1;

    while (!WindowShouldClose()) {
        readTravelerMessages(travelers, numTravelers, scheduler);

        if (allFinished(travelers, numTravelers) && finishTime < 0) {
            finishTime = GetTime();
        }

        if (finishTime > 0 && GetTime() - finishTime > 2.0) {
            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Directed Weighted Graph - Milestone 7", 20, 20, 24, DARKBLUE);

        char schedulerText[50];
        snprintf(schedulerText, sizeof(schedulerText), "Scheduler: %s", schedulerName(scheduler));
        DrawText(schedulerText, 20, 55, 22, MAROON);

        drawEdges(graph, positions, n);
        drawNodes(positions, n);
        drawTravelers(travelers, numTravelers, positions);

        EndDrawing();
    }

    CloseWindow();
}