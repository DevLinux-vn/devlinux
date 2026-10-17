CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

TARGETS = writer_flock writer_fcntl

.PHONY: all test_flock test_fcntl clean

all: $(TARGETS)

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) -o $@ $<

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) -o $@ $<

test_flock: writer_flock
	@echo "Running concurrent test for writer_flock..."
	@for i in $$(seq 1 10); do \
		./writer_flock "flock log message $$i" & \
	done; wait
	@cat system.log

test_fcntl: writer_fcntl
	@echo "Running concurrent test for writer_fcntl..."
	@for i in $$(seq 1 10); do \
		./writer_fcntl "fcntl log message $$i" & \
	done; wait
	@cat system.log

clean:
	rm -f $(TARGETS) system.log

