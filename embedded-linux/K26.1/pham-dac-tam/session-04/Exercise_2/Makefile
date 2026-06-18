CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGETS = manager searcher

all: $(TARGETS)

manager: manager.c
	$(CC) $(CFLAGS) -o manager manager.c

searcher: searcher.c
	$(CC) $(CFLAGS) -o searcher searcher.c

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
