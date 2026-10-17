CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g

all: monitor-daemon monitor-cli

monitor-daemon: server.c
	$(CC) $(CFLAGS) -o monitor-daemon server.c

monitor-cli: client.c
	$(CC) $(CFLAGS) -o monitor-cli client.c

clean:
	rm -f monitor-daemon monitor-cli
	rm -f /tmp/monitor.sock

.PHONY: all clean