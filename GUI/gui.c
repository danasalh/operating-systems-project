#include "gui.h"
#include "../ipc.h"
#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <unistd.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 700
#define NODE_RADIUS 25

static Color travelerColors[] = {
    RED, GREEN, BLUE, ORANGE, PURPLE, MAROON, DARKGREEN, DARKBLUE
};

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

static void readTravelerMessages(Traveler* travelers, int numTravelers) {
    for (int i = 0; i < numTravelers; i++) {
        TravelerMessage msg;

        while (read(travelers[i].pipeFd[0], &msg, sizeof(TravelerMessage)) > 0) {
            travelers[i].currentNode = msg.currentNode;
            travelers[i].nextNode = msg.nextNode;
            travelers[i].finished = msg.finished;

            if (msg.finished) {
                printf("[PID=%d] arrived at node %d | DESTINATION\n",
                       msg.pid, msg.currentNode);
                printf("[PID=%d] finished\n", msg.pid);
            } else {
                printf("[PID=%d] arrived at node %d | next node: %d\n",
                       msg.pid, msg.currentNode, msg.nextNode);
            }

            fflush(stdout);
        }
    }
}

static void drawTravelers(Traveler* travelers, int numTravelers, Vector2 positions[]) {
    for (int i = 0; i < numTravelers; i++) {
        int node = travelers[i].currentNode;

        if (node < 0) {
            continue;
        }

        Color color = travelerColors[i % 8];
        Vector2 pos = positions[node];

        DrawCircleV(pos, 13, color);

        char text[20];
        snprintf(text, sizeof(text), "T%d", i);
        DrawText(text, pos.x - 10, pos.y - 10, 16, WHITE);
    }
}

void drawGraph(Graph* graph, Traveler* travelers, int numTravelers) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Milestone 5 - IPC Graph Simulation");
    SetTargetFPS(60);

    int n = graph->numVertices;
    Vector2 positions[15];

    calculatePositions(positions, n);

    while (!WindowShouldClose()) {
        readTravelerMessages(travelers, numTravelers);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Directed Weighted Graph - Milestone 5", 20, 20, 24, DARKBLUE);

        drawEdges(graph, positions, n);
        drawNodes(positions, n);
        drawTravelers(travelers, numTravelers, positions);

        EndDrawing();
    }

    CloseWindow();
}