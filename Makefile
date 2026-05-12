CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = graph_project

SRCS = main.c graph.c dijkstra.c

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)