CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGETS = manager searcher

.PHONY: all clean

all: $(TARGETS)

manager: manager.c
	$(CC) $(CFLAGS) -o manager manager.c

searcher: searcher.c
	$(CC) $(CFLAGS) -o searcher searcher.c

clean:
	rm -f $(TARGETS)

