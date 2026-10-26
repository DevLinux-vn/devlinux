CC := gcc
CFLAGS := -Wall -Wextra -std=gnu99

# Build both manager and searcher[cite: 2]
all: manager searcher

manager: manager.c
	$(CC) $(CFLAGS) $< -o $@

searcher: searcher.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f manager searcher