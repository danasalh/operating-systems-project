#ifndef TRAVELER_H
#define TRAVELER_H

#include <sys/types.h>

typedef struct {
    int source;
    int destination;
    int* path;
    int pathLength;
    int totalWeight;
    pid_t pid;
} Traveler;

#endif