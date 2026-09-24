CC := gcc
CFLAGS := -Wall -Wextra -std=c11
TARGETS := writer_flock writer_fcntl

all: $(TARGETS)

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) $< -o $@

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) $< -o $@

test_flock: writer_flock
	rm -f system.log
	for i in 1 2 3 4 5; do ./writer_flock "flock message $$i" & done; wait
	cat system.log

test_fcntl: writer_fcntl
	rm -f system.log
	for i in 1 2 3 4 5; do ./writer_fcntl "fcntl message $$i" & done; wait
	cat system.log

clean:
	rm -f $(TARGETS)

.PHONY: all test_flock test_fcntl clean
