CC = gcc
CFLAGS = -Wall -Wextra -std=c11

.PHONY: all test_flock test_fcntl clean

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) -o writer_flock writer_flock.c

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) -o writer_fcntl writer_fcntl.c

test_flock: writer_flock
	rm -f system.log
	@for i in 1 2 3 4 5 6 7 8 9 10; do \
		./writer_flock "message tu process $$i" & \
	done; \
	wait
	@echo "----- Noi dung system.log sau test_flock -----"
	@cat system.log
	@echo "----- So dong ghi duoc: $$(wc -l < system.log) (ky vong 10) -----"

test_fcntl: writer_fcntl
	rm -f system.log
	@for i in 1 2 3 4 5 6 7 8 9 10; do \
		./writer_fcntl "message tu process $$i" & \
	done; \
	wait
	@echo "----- Noi dung system.log sau test_fcntl -----"
	@cat system.log
	@echo "----- So dong ghi duoc: $$(wc -l < system.log) (ky vong 10) -----"

clean:
	rm -f writer_flock writer_fcntl system.log