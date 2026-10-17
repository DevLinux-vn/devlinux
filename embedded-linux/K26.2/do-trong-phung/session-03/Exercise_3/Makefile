CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGETS = writer_flock writer_fcntl

.PHONY: all test_flock test_fcntl clean

all: $(TARGETS)

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) -o $@ $<

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) -o $@ $<

test_flock: writer_flock
	rm -f system.log
	./writer_flock "Message from process 1" &
	./writer_flock "Message from process 2" &
	./writer_flock "Message from process 3" &
	./writer_flock "Message from process 4" &
	./writer_flock "Message from process 5" &
	wait
	cat system.log

test_fcntl: writer_fcntl
	rm -f system.log
	./writer_fcntl "Message from process 1" &
	./writer_fcntl "Message from process 2" &
	./writer_fcntl "Message from process 3" &
	./writer_fcntl "Message from process 4" &
	./writer_fcntl "Message from process 5" &
	wait
	cat system.log

clean:
	rm -f $(TARGETS) system.log