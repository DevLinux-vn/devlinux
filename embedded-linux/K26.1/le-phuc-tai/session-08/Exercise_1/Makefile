CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu99 -g

all: monitor-daemon monitor-cli

monitor-daemon: server.c
	$(CC) $(CFLAGS) -o monitor-daemon server.c

monitor-cli: client.c
	$(CC) $(CFLAGS) -o monitor-cli client.c

clean:
	rm -f monitor-daemon monitor-cli

.PHONY: all clean