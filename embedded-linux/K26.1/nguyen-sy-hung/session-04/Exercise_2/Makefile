CC      = gcc

CFLAGS  = -Wall -Wextra -Werror -std=c11

TARGETS = manager searcher


all: $(TARGETS)


manager: manager.c
	$(CC) $(CFLAGS) manager.c -o manager


searcher: searcher.c
	$(CC) $(CFLAGS) searcher.c -o searcher


clean:
	rm -f $(TARGETS)