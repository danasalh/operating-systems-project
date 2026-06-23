#ifndef OPERATING_SYSTEMS_PROJECT_PROCESS_H
#define OPERATING_SYSTEMS_PROJECT_PROCESS_H

typedef struct {
    int pid;
    int arrival;
    int burst;
    int waiting;
    int turnaround;
} Process;

#endif //OPERATING_SYSTEMS_PROJECT_PROCESS_H