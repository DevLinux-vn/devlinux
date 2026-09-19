CC = gcc
CFLAGS = -Wall -g

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) writer_flock.c -o writer_flock

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) writer_fcntl.c -o writer_fcntl

test_flock: writer_flock
	./writer_flock "Message 1" &
	./writer_flock "Message 2" &
	./writer_flock "Message 3" &
	wait

test_fcntl: writer_fcntl
	./writer_fcntl "Message 1" &
	./writer_fcntl "Message 2" &
	./writer_fcntl "Message 3" &
	wait

clean:
	rm -f writer_flock writer_fcntl system.log