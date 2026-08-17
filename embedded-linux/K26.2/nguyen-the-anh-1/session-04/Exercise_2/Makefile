CC     = gcc
CFLAGS = -Wall -Wextra -g

all: manager searcher

manager: manager.c
	$(CC) $(CFLAGS) manager.c -o manager

searcher: searcher.c
	$(CC) $(CFLAGS) searcher.c -o searcher

clean:
	rm -f manager searcher

.PHONY: all clean
