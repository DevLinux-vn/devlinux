CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
TARGETS = daemon cli

all: $(TARGETS)

daemon: daemon.c
	$(CC) $(CFLAGS) -o $@ $<

cli: cli.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean