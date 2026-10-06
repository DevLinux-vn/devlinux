CC := gcc
CFLAGS := -Wall -Wextra -std=gnu99 -pthread

all: print_queue

print_queue: print_queue.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f print_queue