# Compiler Configuration
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -pedantic -std=c99 -O2

# Build Targets
TARGETS := daemon cli

.PHONY: all clean

all: $(TARGETS)

daemon: daemon.c
	$(CC) $(CFLAGS) -o $@ $<

cli: cli.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) *.o
	