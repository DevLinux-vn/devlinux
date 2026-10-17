CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGETS = writer_flock writer_fcntl

all: $(TARGETS)

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) -o $@ $^

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) -o $@ $^

test_flock: writer_flock
	./writer_flock "Test message from flock 1" & \
	./writer_flock "Test message from flock 2" & \
	wait
	cat system.log

test_fcntl: writer_fcntl
	./writer_fcntl "Test message from fcntl 1" & \
	./writer_fcntl "Test message from fcntl 2" & \
	wait
	cat system.log

clean:
	rm -f $(TARGETS) system.log
