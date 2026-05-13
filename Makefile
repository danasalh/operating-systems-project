CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = graph_project

SRCS = main.c graph.c dijkstra.c GUI/gui.c

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

clean:
	rm -f $(TARGET)