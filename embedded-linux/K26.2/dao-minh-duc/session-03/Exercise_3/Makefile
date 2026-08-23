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
	@rm -f system.log
	@echo "Testing flock..."
	@./writer_flock "message from flock 1"
	@./writer_flock "message from flock 2"
	@./writer_flock "message from flock 3"
	@echo "--- system.log ---"
	@cat system.log

test_fcntl: writer_fcntl
	@rm -f system.log
	@echo "Testing fcntl..."
	@./writer_fcntl "message from fcntl 1"
	@./writer_fcntl "message from fcntl 2"
	@./writer_fcntl "message from fcntl 3"
	@echo "--- system.log ---"
	@cat system.log

clean:
	rm -f $(TARGETS) system.log
