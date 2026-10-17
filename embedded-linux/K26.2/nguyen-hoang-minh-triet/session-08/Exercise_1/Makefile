# Compiler settings
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -pedantic -std=c99 -O2

# Targets
TARGETS := server client

.PHONY: all clean

all: $(TARGETS)

server: server.c
	$(CC) $(CFLAGS) -o $@ $<

client: client.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) *.o /tmp/monitor.sock

