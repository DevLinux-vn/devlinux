CC = gcc
CFLAGS = -Wall -Wextra

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) -o writer_flock writer_flock.c

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) -o writer_fcntl writer_fcntl.c

test_flock: writer_flock
	./writer_flock "Test message from flock process 1" &
	./writer_flock "Test message from flock process 2" &
	./writer_flock "Test message from flock process 3" &
	sleep 1
	cat system.log

test_fcntl: writer_fcntl
	./writer_fcntl "Test message from fcntl process A" &
	./writer_fcntl "Test message from fcntl process B" &
	./writer_fcntl "Test message from fcntl process C" &
	sleep 1
	cat system.log

clean:
	rm -f writer_flock writer_fcntl system.log