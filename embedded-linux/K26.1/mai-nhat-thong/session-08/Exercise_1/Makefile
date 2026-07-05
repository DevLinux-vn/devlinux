CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -std=gnu99
TARGETS = monitor-daemon monitor-cli

all: $(TARGETS)

monitor-daemon: server.c
	$(CC) $(CFLAGS) -o $@ $<

monitor-cli: client.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean