#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "traveler.h"

#define MAX_PROCESSES 100


static int readProcessesFromFile(FILE* file, Traveler travelers[])
{
    int numTravelers;

    if (fscanf(file, "%d", &numTravelers) != 1)
        return -1;

    if (numTravelers < 0)
        return -1;

    for (int i = 0; i < numTravelers; i++)
    {
        if (fscanf(file, "%d %d %d",
                   &travelers[i].pid,
                   &travelers[i].arrival,
                   &travelers[i].burst) != 3)
        {
            return -1;
        }
    }

    return numTravelers;
}


void runFCFS(Traveler travelers[], int processCount)
{
    int currentTime = 0;

    printf("FCFS Scheduling\n\n");

    for (int i = 0; i < processCount; i++)
    {
        if (currentTime < travelers[i].arrival)
        {
            currentTime = travelers[i].arrival;
        }

        travelers[i].waiting =
            currentTime - travelers[i].arrival;

        currentTime += travelers[i].burst;

        travelers[i].turnaround =
            currentTime - travelers[i].arrival;
    }

    printf("PID\tArrival\tBurst\tWaiting\tTurnaround\n");

    for (int i = 0; i < processCount; i++)
    {
        printf("%d\t%d\t%d\t%d\t%d\n",
               travelers[i].pid,
               travelers[i].arrival,
               travelers[i].burst,
               travelers[i].waiting,
               travelers[i].turnaround);
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
    Traveler travelers[MAX_PROCESSES];
    FILE* file = fopen(argv[2], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Cannot open file\n"); return 1;
    }
    int processCount = readProcessesFromFile(file, travelers);
    if (strcmp(argv[1], "fcfs") == 0) {
        runFCFS(travelers, processCount);
    }

    /*else if (strcmp(argv[1], "sjf") == 0) {
     *runSJF(processes, processCount);
     *}
     *return 0; } */
}