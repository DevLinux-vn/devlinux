CC = gcc
CFLAGS=-Wall -Wextra -std=c99 -Iinclude -g

all: test_flock test_fcntl

test_flock:
	$(CC) $(CFLAGS) writer_flock.c -o writer_flock

test_fcntl:
	$(CC) $(CFLAGS) writer_fcntl.c -o writer_fcntl

.PHONY: clean run all
clean:
	rm writer_flock
	rm writer_fcntl
	rm system.log
