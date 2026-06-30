#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100

typedef struct {
    int pid;
    int arrival;
    int burst;
    int waiting;
    int turnaround;
    int completed;
} Process;

static int readProcessesFromFile(FILE* file, Process processes[]) {
    int n;

    if (fscanf(file, "%d", &n) != 1) {
        return -1;
    }

    if (n <= 0 || n > MAX_PROCESSES) {
        return -1;
    }

    for (int i = 0; i < n; i++) {
        if (fscanf(file, "%d %d %d",
                   &processes[i].pid,
                   &processes[i].arrival,
                   &processes[i].burst) != 3) {
            return -1;
        }

        processes[i].waiting = 0;
        processes[i].turnaround = 0;
        processes[i].completed = 0;
    }

    return n;
}

static void printResults(Process processes[], int n, const char* schedulerName) {
    double totalWaiting = 0;
    double totalTurnaround = 0;

    printf("\nScheduler: %s\n\n", schedulerName);
    printf("PID\tArrival\tBurst\tWaiting\tTurnaround\n");

    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n",
               processes[i].pid,
               processes[i].arrival,
               processes[i].burst,
               processes[i].waiting,
               processes[i].turnaround);

        totalWaiting += processes[i].waiting;
        totalTurnaround += processes[i].turnaround;
    }

    printf("\nAverage waiting time: %.2f\n", totalWaiting / n);
    printf("Average turnaround time: %.2f\n", totalTurnaround / n);
}

static void runFCFS(Process processes[], int n) {
    int currentTime = 0;

    for (int i = 0; i < n; i++) {
        if (currentTime < processes[i].arrival) {
            currentTime = processes[i].arrival;
        }

        printf("Time %d: Process %d selected\n",
               currentTime,
               processes[i].pid);

        processes[i].waiting = currentTime - processes[i].arrival;

        currentTime += processes[i].burst;

        processes[i].turnaround = currentTime - processes[i].arrival;

        printf("Time %d: Process %d completed\n",
               currentTime,
               processes[i].pid);
    }

    printResults(processes, n, "FCFS");
}

static void runSJF(Process processes[], int n) {
    int currentTime = 0;
    int completedCount = 0;

    while (completedCount < n) {
        int selected = -1;
        int shortestBurst = 1000000000;

        for (int i = 0; i < n; i++) {
            if (!processes[i].completed &&
                processes[i].arrival <= currentTime &&
                processes[i].burst < shortestBurst) {

                shortestBurst = processes[i].burst;
                selected = i;
            }
        }

        if (selected == -1) {
            currentTime++;
            continue;
        }

        printf("Time %d: Process %d selected\n",
               currentTime,
               processes[selected].pid);

        processes[selected].waiting =
            currentTime - processes[selected].arrival;

        currentTime += processes[selected].burst;

        processes[selected].turnaround =
            currentTime - processes[selected].arrival;

        processes[selected].completed = 1;
        completedCount++;

        printf("Time %d: Process %d completed\n",
               currentTime,
               processes[selected].pid);
    }

    printResults(processes, n, "SJF");
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s -schd fcfs <input_file>\n", argv[0]);
        printf("Usage: %s -schd sjf <input_file>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-schd") != 0) {
        printf("Invalid option. Use -schd\n");
        return 1;
    }

    FILE* file = fopen(argv[3], "r");
    if (file == NULL) {
        printf("Cannot open file\n");
        return 1;
    }

    Process processes[MAX_PROCESSES];

    int processCount = readProcessesFromFile(file, processes);
    fclose(file);

    if (processCount <= 0) {
        printf("Invalid input\n");
        return 1;
    }

    if (strcmp(argv[2], "fcfs") == 0) {
        runFCFS(processes, processCount);
    }
    else if (strcmp(argv[2], "sjf") == 0) {
        runSJF(processes, processCount);
    }
    else {
        printf("Unknown scheduler: %s\n", argv[2]);
        return 1;
    }

    return 0;
}