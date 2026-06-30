#ifndef GUI_H
#define GUI_H

#include "../graph.h"
#include "../traveler.h"
#include "../ipc.h"

void drawGraph(Graph* graph, Traveler* travelers, int numTravelers, SchedulerType scheduler);

#endif