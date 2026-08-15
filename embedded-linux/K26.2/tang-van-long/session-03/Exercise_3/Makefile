CUR_DIR :=.
FLOCK_DIR := $(CUR_DIR)/writer_flock
FCNTL_DIR := $(CUR_DIR)/writer_fcntl

CC := gcc

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) writer_flock.c -o writer_flock

writer_fcntl: writer_fcntl.c
	$(CC) writer_fcntl.c -o writer_fcntl

test_flock: writer_flock
	$(FLOCK_DIR) "Message 1" &
	$(FLOCK_DIR) "Message 2" &
	$(FLOCK_DIR) "Message 3" &
	$(FLOCK_DIR) "Message 4" &
	$(FLOCK_DIR) "Message 5" &
	wait

test_fcntl: writer_fcntl
	$(FCNTL_DIR) "Message 1" &
	$(FCNTL_DIR) "Message 2" &
	$(FCNTL_DIR) "Message 3" &
	$(FCNTL_DIR) "Message 4" &
	$(FCNTL_DIR) "Message 5" &
	wait

clean:
	rm -rf writer_flock writer_fcntl system.log