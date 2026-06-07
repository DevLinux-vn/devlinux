CC      = gcc
CFLAGS  = -Wall -Wextra

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) -o writer_flock writer_flock.c

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) -o writer_fcntl writer_fcntl.c

test_flock: writer_flock
	rm -f system.log
	for i in 1 2 3 4 5; do ./writer_flock "flock message $$i" & done; wait
	@echo "--- system.log (flock) ---"
	@cat system.log

test_fcntl: writer_fcntl
	rm -f system.log
	for i in 1 2 3 4 5; do ./writer_fcntl "fcntl message $$i" & done; wait
	@echo "--- system.log (fcntl) ---"
	@cat system.log

clean:
	rm -f writer_flock writer_fcntl system.log
