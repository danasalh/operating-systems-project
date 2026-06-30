#ifndef IPC_H
#define IPC_H

#include <sys/types.h>

#define MSG_REQUEST 1
#define MSG_ENTERED 2
#define MSG_LEFT 3

typedef enum {
    SCHED_FCFS,
    SCHED_SJF
} SchedulerType;

typedef struct {
    pid_t pid;
    int travelerIndex;
    int currentNode;
    int nextNode;
    int finished;
    int waiting;
    int remainingCost;
    int type;
} TravelerMessage;

#endif