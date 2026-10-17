CC = gcc
CFLAGS = -Wall -Wextra -g

TARGETS = daemon cli

all: $(TARGETS)

daemon: daemon.c
	$(CC) $(CFLAGS) -o daemon daemon.c

cli: cli.c
	$(CC) $(CFLAGS) -o cli cli.c

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
