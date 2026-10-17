CC=gcc
CFLAGS=-Wall -Wextra -std=c11

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) writer_flock.c -o writer_flock

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) writer_fcntl.c -o writer_fcntl

test_flock:
	./writer_flock "message from flock"

test_fcntl:
	./writer_fcntl "message from fcntl"

clean:
	rm -f writer_flock writer_fcntl system.log