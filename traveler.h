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
    int pipeFd[2];
    int controlFd[2];

    int currentNode;
    int nextNode;
    int requestedNode;
    int finished;
    int waiting;
    int completed;

    /* scheduling */
    int arrival;
    int burst;
    int turnaround;

} Traveler;



#endif
