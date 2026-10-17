CC     = gcc
CFLAGS = -Wall -Wextra -g

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) writer_flock.c -o writer_flock

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) writer_fcntl.c -o writer_fcntl

test_flock: writer_flock
	@rm -f system.log
	@for i in $$(seq 1 20); do ./writer_flock "flock message $$i" & done; wait
	@echo "---- system.log ----"
	@cat system.log
	@echo "---- line count (expect 20) ----"
	@wc -l < system.log

test_fcntl: writer_fcntl
	@rm -f system.log
	@for i in $$(seq 1 20); do ./writer_fcntl "fcntl message $$i" & done; wait
	@echo "---- system.log ----"
	@cat system.log
	@echo "---- line count (expect 20) ----"
	@wc -l < system.log

clean:
	rm -f writer_flock writer_fcntl system.log

.PHONY: all test_flock test_fcntl clean
