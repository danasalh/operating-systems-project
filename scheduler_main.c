#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "process.h"

#define MAX_PROCESSES 100


static int readProcessesFromFile(FILE* file, Process processes[])
{
    int numProcesses;

    if (fscanf(file, "%d", &numProcesses) != 1)
        return -1;

    if (numProcesses < 0)
        return -1;

    for (int i = 0; i < numProcesses; i++)
    {
        if (fscanf(file, "%d %d %d",
                   &processes[i].pid,
                   &processes[i].arrival,
                   &processes[i].burst) != 3)
        {
            return -1;
        }
    }

    return numProcesses;
}


void runFCFS(Process processes[], int processCount)
{
    int currentTime = 0;

    printf("FCFS Scheduling\n\n");

    for (int i = 0; i < processCount; i++)
    {
        if (currentTime < processes[i].arrival)
        {
            currentTime = processes[i].arrival;
        }

        processes[i].waiting =
            currentTime - processes[i].arrival;

        currentTime += processes[i].burst;

        processes[i].turnaround =
            currentTime - processes[i].arrival;
    }

    printf("PID\tArrival\tBurst\tWaiting\tTurnaround\n");

    for (int i = 0; i < processCount; i++)
    {
        printf("%d\t%d\t%d\t%d\t%d\n",
               processes[i].pid,
               processes[i].arrival,
               processes[i].burst,
               processes[i].waiting,
               processes[i].turnaround);
    }
}


int main(int argc, char* argv[]) {
    if (argc != 3)
    {
        fprintf(stderr,
                "Usage: %s fcfs input.txt\n",
                argv[0]);
        return 1;
    }
    Process processes[MAX_PROCESSES];
    FILE* file = fopen(argv[2], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Cannot open file\n"); return 1;
    }
    int processCount = readProcessesFromFile(file, processes);
    if (strcmp(argv[1], "fcfs") == 0) {
        runFCFS(processes, processCount);
    }

    /*else if (strcmp(argv[1], "sjf") == 0) {
     *runSJF(processes, processCount);
     *}
     *return 0; */ }
}