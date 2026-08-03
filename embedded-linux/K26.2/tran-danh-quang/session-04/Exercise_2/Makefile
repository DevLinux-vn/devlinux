CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

TARGETS = manager searcher
SRCS_MANAGER = manager.c
SRCS_SEARCHER = searcher.c

all: $(TARGETS)

manager: $(SRCS_MANAGER)
	$(CC) $(CFLAGS) -o $@ $^

searcher: $(SRCS_SEARCHER)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)

.PHONY: all clean
