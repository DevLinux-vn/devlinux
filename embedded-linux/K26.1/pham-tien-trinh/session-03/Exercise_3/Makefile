CC = gcc
CFLAGS = -Wall -Wextra

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) writer_flock.c -o writer_flock

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) writer_fcntl.c -o writer_fcntl

test_flock: writer_flock
	rm -f system.log
	for i in $$(seq 1 10); do ./writer_flock "message $$i" & done; wait
	wc -l system.log
	grep -c "^\[PID" system.log

test_fcntl: writer_fcntl
	rm -f system.log
	for i in $$(seq 1 10); do ./writer_fcntl "message $$i" & done; wait
	wc -l system.log
	grep -c "^\[PID" system.log

clean:
	rm -f writer_flock writer_fcntl system.log