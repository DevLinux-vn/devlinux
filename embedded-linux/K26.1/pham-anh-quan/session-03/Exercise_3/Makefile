CC = gcc
CFLAGS = -Wall -Wextra

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) writer_flock.c -o writer_flock

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) writer_fcntl.c -o writer_fcntl

test_flock: writer_flock
	@echo "Testing flock with multiple processes..."
	./writer_flock "Hello from flock 1" & \
	./writer_flock "Hello from flock 2" & \
	./writer_flock "Hello from flock 3" & \
	wait
	@cat system.log

test_fcntl: writer_fcntl
	@echo "Testing fcntl with multiple processes..."
	./writer_fcntl "Hello from fcntl 1" & \
	./writer_fcntl "Hello from fcntl 2" & \
	./writer_fcntl "Hello from fcntl 3" & \
	wait
	@cat system.log

clean:
	rm -f writer_flock writer_fcntl system.log

.PHONY: all test_flock test_fcntl clean