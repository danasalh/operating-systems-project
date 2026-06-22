#include "gui.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

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

                Vector2 mid = {
                    (positions[i].x + positions[j].x) / 2.0f,
                    (positions[i].y + positions[j].y) / 2.0f
                };

                char weightText[20];
                sprintf(weightText, "%d", graph->matrix[i][j]);
                DrawText(weightText, mid.x, mid.y, 18, RED);
            }
        }
    }
}

static void drawNodes(Vector2 positions[], int n) {
    for (int i = 0; i < n; i++) {
        DrawCircleV(positions[i], NODE_RADIUS, SKYBLUE);
        DrawCircleLines(positions[i].x, positions[i].y, NODE_RADIUS, DARKBLUE);

        char label[20];
        sprintf(label, "%d", i);
        DrawText(label, positions[i].x - 5, positions[i].y - 10, 20, BLACK);
    }
}

static void drawTravelers(Graph* graph, Traveler* travelers, int numTravelers, Vector2 positions[]) {
    double time = GetTime();
    double speedFactor = 1.0;   
    for (int i = 0; i < numTravelers; i++) {
        if (travelers[i].totalWeight == INF || travelers[i].pathLength == 0) {
            continue;
        }

        Color color = travelerColors[i % 8];

        Vector2 travelerPosition = positions[travelers[i].path[0]];
        double elapsed = time;

        for (int j = 0; j < travelers[i].pathLength - 1; j++) {
            int from = travelers[i].path[j];
            int to = travelers[i].path[j + 1];
            int weight = graph->matrix[from][to];

            double segmentTime = weight * speedFactor;

            if (elapsed <= segmentTime) {
                float t = (float)(elapsed / segmentTime);

                travelerPosition.x = positions[from].x +
                    t * (positions[to].x - positions[from].x);

                travelerPosition.y = positions[from].y +
                    t * (positions[to].y - positions[from].y);

                break;
            }

            elapsed -= segmentTime;
            travelerPosition = positions[to];
        }

        DrawCircleV(travelerPosition, 12, color);

        char text[20];
        snprintf(text, sizeof(text), "T%d", i);
        DrawText(text, travelerPosition.x - 10, travelerPosition.y - 10, 16, WHITE);
    }
}

void drawGraph(Graph* graph, Traveler* travelers, int numTravelers) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Graph GUI - Multiple Travelers");
    SetTargetFPS(60);

    int n = graph->numVertices;
    Vector2 positions[15];

    calculatePositions(positions, n);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Directed Weighted Graph - Milestone 4", 20, 20, 24, DARKBLUE);

        drawEdges(graph, positions, n);
        drawNodes(positions, n);
        drawTravelers(graph, travelers, numTravelers, positions);

        EndDrawing();
    }

    CloseWindow();
}