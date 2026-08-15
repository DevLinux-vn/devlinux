CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -O2 -g

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) writer_flock.c -o writer_flock

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) writer_fcntl.c -o writer_fcntl

test_flock: writer_flock
	./writer_flock "Message 1" &
	./writer_flock "Message 2" &
	./writer_flock "Message 3" &
	./writer_flock "Message 4" &
	./writer_flock "Message 5" &
	wait

test_fcntl: writer_fcntl
	./writer_fcntl "Message 1" &
	./writer_fcntl "Message 2" &
	./writer_fcntl "Message 3" &
	./writer_fcntl "Message 4" &
	./writer_fcntl "Message 5" &
	wait

clean:
	rm -rf writer_flock writer_fcntl system.log