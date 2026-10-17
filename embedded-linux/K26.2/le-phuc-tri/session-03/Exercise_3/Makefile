CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET_FLOCK = writer_flock
TARGET_FCNTL = writer_fcntl

.PHONY: all test_flock test_fcntl clean

all: $(TARGET_FLOCK) $(TARGET_FCNTL)

$(TARGET_FLOCK): writer_flock.c
	$(CC) $(CFLAGS) writer_flock.c -o $(TARGET_FLOCK)

$(TARGET_FCNTL): writer_fcntl.c
	$(CC) $(CFLAGS) writer_fcntl.c -o $(TARGET_FCNTL)

test_flock: $(TARGET_FLOCK)
	rm -f system.log
	./writer_flock "Message from process 1" &
	./writer_flock "Message from process 2" &
	./writer_flock "Message from process 3" &
	./writer_flock "Message from process 4" &
	./writer_flock "Message from process 5" &
	wait
	cat system.log

test_fcntl: $(TARGET_FCNTL)
	rm -f system.log
	./writer_fcntl "Message from process 1" &
	./writer_fcntl "Message from process 2" &
	./writer_fcntl "Message from process 3" &
	./writer_fcntl "Message from process 4" &
	./writer_fcntl "Message from process 5" &
	wait
	cat system.log

clean:
	rm -f $(TARGET_FLOCK) $(TARGET_FCNTL) system.log
