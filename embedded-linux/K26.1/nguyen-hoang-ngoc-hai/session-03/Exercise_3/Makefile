CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGETS = writer_flock writer_fcntl

all: $(TARGETS)

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) writer_flock.c -o writer_flock

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) writer_fcntl.c -o writer_fcntl

test_flock: writer_flock
	rm -f system.log
	./writer_flock "flock message 1" &
	./writer_flock "flock message 2" &
	./writer_flock "flock message 3" &
	./writer_flock "flock message 4" &
	wait
	cat system.log

test_fcntl: writer_fcntl
	rm -f system.log
	./writer_fcntl "fcntl message 1" &
	./writer_fcntl "fcntl message 2" &
	./writer_fcntl "fcntl message 3" &
	./writer_fcntl "fcntl message 4" &
	wait
	cat system.log

clean:
	rm -f $(TARGETS) system.log

.PHONY: all test_flock test_fcntl clean