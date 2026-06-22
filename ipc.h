#ifndef IPC_H
#define IPC_H

#include <sys/types.h>

typedef struct {
    pid_t pid;
    int travelerIndex;
    int currentNode;
    int nextNode;
    int finished;
    int waiting;

} TravelerMessage;

#endif