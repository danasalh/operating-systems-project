#ifndef IPC_H
#define IPC_H

#include <sys/types.h>

typedef enum {
    MSG_REQUEST_NODE,
    MSG_ARRIVED_NODE,
    MSG_LEFT_NODE,
    MSG_FINISHED
} MessageType;

typedef struct {
    MessageType type;
    pid_t pid;
    int travelerIndex;
    int currentNode;
    int nextNode;
    int finished;
    int waiting;

} TravelerMessage;

typedef struct {
    int granted;
    int node;
} TravelerCommand;

#endif
