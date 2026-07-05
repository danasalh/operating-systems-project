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
    
    int pipeFd[2];      // Child -> Parent
int ackPipeFd[2];   // Parent -> Child

    int currentNode;
    int nextNode;
    int finished;
} Traveler;

#endif