CC := gcc
CFLAGS := -Wall -Wextra -std=c11
TARGETS := manager searcher

all: $(TARGETS)

manager: manager.c
	$(CC) $(CFLAGS) $< -o $@

searcher: searcher.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean
