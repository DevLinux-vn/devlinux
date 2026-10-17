CC=gcc
CFLAGS=-Wall -Wextra -pthread

all: print_queue

print_queue: print_queue.c
	$(CC) $(CFLAGS) print_queue.c -o print_queue

clean:
	rm -f print_queue