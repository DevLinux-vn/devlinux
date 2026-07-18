CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

all: manager searcher

manager: manager.c
	$(CC) $(CFLAGS) -o manager manager.c

searcher: searcher.c
	$(CC) $(CFLAGS) -o searcher searcher.c

clean:
	rm -f manager searcher