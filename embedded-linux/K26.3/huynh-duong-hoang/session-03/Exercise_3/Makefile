CC = gcc
CFLAGS = -Wall -Wextra -g

TARGETS = writer_flock writer_fcntl
LOG_FILE = system.log

all: $(TARGETS)

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) $< -o $@

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) $< -o $@

test_flock: writer_flock
	@echo "Testing writer_flock concurrently..."
	@./writer_flock "flock process 1 log entry" & \
	 ./writer_flock "flock process 2 log entry" & \
	 ./writer_flock "flock process 3 log entry" & \
	 wait
	@cat $(LOG_FILE)

test_fcntl: writer_fcntl
	@echo "Testing writer_fcntl concurrently..."
	@./writer_fcntl "fcntl process 1 log entry" & \
	 ./writer_fcntl "fcntl process 2 log entry" & \
	 ./writer_fcntl "fcntl process 3 log entry" & \
	 wait
	@cat $(LOG_FILE)

clean:
	rm -f $(TARGETS) $(LOG_FILE)

.PHONY: all test_flock test_fcntl clean
