CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = sim

SRCS = scheduler_main.c graph.c dijkstra.c GUI/gui.c

LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: milestone6

milestone4:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

milestone5:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

milestone6:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

milestone7:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)


clean:
	rm -f $(TARGET) graph_project