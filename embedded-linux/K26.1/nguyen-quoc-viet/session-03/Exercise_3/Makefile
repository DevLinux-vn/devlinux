CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGETS = writer_flock writer_fcntl

all: $(TARGETS)

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) -o writer_flock writer_flock.c

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) -o writer_fcntl writer_fcntl.c

test_flock: writer_flock
	./writer_flock "Test message from flock 1"
	./writer_flock "Test message from flock 2"
	@echo "=== Log file content ==="
	@cat system.log

test_fcntl: writer_fcntl
	./writer_fcntl "Test message from fcntl 1"
	./writer_fcntl "Test message from fcntl 2"
	@echo "=== Log file content ==="
	@cat system.log

clean:
	rm -f $(TARGETS) system.log

.PHONY: all test_flock test_fcntl clean
