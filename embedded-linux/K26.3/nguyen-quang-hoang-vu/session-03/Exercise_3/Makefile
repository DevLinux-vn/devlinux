CC=gcc
CFLAGS=-Wall -Wextra

all: writer_flock writer_fcntl

writer_flock:
	$(CC) $(CFLAGS) writer_flock.c -o writer_flock

writer_fcntl:
	$(CC) $(CFLAGS) writer_fcntl.c -o writer_fcntl

test_flock:
	./writer_flock "hello flock"

test_fcntl:
	./writer_fcntl "hello fcntl"

clean:
	rm -f writer_flock writer_fcntl system.log
