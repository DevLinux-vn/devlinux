CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGETS = monitor-daemon monitor-cli

all: $(TARGETS)

monitor-daemon: server.c
	$(CC) $(CFLAGS) server.c -o monitor-daemon

monitor-cli: client.c
	$(CC) $(CFLAGS) client.c -o monitor-cli

clean:
	rm -f $(TARGETS)
	rm -f /tmp/monitor.sock
