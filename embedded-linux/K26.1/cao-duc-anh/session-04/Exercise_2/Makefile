CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -std=gnu11

all: manager searcher

manager: manager.c
	$(CC) $(CFLAGS) -o manager manager.c

searcher: searcher.c
	$(CC) $(CFLAGS) -o searcher searcher.c

clean:
	rm -f manager searcher

.PHONY: all clean
