CC := gcc
CFLAGS := -Wall -Wextra -std=gnu99

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) $< -o $@

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) $< -o $@

test_flock: writer_flock
	@echo "--- Testing flock concurrency ---"
	@rm -f system.log
	@for i in 1 2 3 4 5; do \
		./writer_flock "Message $$i from flock" & \
	done
	@wait
	@echo "--- system.log output ---"
	@cat system.log

test_fcntl: writer_fcntl
	@echo "--- Testing fcntl concurrency ---"
	@rm -f system.log
	@for i in 1 2 3 4 5; do \
		./writer_fcntl "Message $$i from fcntl" & \
	done
	@wait
	@echo "--- system.log output ---"
	@cat system.log

clean:
	rm -f writer_flock writer_fcntl system.log