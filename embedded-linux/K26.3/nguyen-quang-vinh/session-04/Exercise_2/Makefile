CC = gcc
CFLAGS = -Wall -Wextra

all: manager searcher

manager: manager.c
	$(CC) $(CFLAGS) manager.c -o manager

searcher: searcher.c
	$(CC) $(CFLAGS) searcher.c -o searcher

run: all
	./manager

clean:
	rm -f manager searchers