#ifndef GUI_H
#define GUI_H

#include "../graph.h"
#include "../traveler.h"

typedef enum {
    SCHEDULER_FCFS,
    SCHEDULER_SJF
} SchedulerAlgorithm;

void drawGraph(Graph* graph,
               Traveler* travelers,
               int numTravelers,
               SchedulerAlgorithm scheduler);

#endif
