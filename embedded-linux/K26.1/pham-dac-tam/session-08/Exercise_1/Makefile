CC = gcc
CFLAGS = -Wall -Wextra -g

TARGETS = monitor-daemon monitor-cli

all: $(TARGETS)

monitor-daemon: server.c
	$(CC) $(CFLAGS) -o monitor-daemon server.c

monitor-cli: client.c
	$(CC) $(CFLAGS) -o monitor-cli client.c

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
