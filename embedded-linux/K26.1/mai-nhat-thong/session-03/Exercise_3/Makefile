CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=gnu99 -O2 -g
TARGET_FLOCK = writer_flock
TARGET_FCNTL = writer_fcntl

all: $(TARGET_FLOCK) $(TARGET_FCNTL)

$(TARGET_FLOCK): writer_flock.c
	$(CC) $(CFLAGS) writer_flock.c -o $(TARGET_FLOCK)

$(TARGET_FCNTL): writer_fcntl.c
	$(CC) $(CFLAGS) writer_fcntl.c -o $(TARGET_FCNTL)

test_flock: $(TARGET_FLOCK)
	@echo "Testing flock concurrency..."
	./$(TARGET_FLOCK) "Msg 1 from flock" & \
	./$(TARGET_FLOCK) "Msg 2 from flock" & \
	./$(TARGET_FLOCK) "Msg 3 from flock" & \
	./$(TARGET_FLOCK) "Msg 4 from flock" & \
	./$(TARGET_FLOCK) "Msg 5 from flock" & \
	wait
	@echo "--- Resulting system.log (flock) ---"
	@cat system.log

test_fcntl: $(TARGET_FCNTL)
	@echo "Testing fcntl concurrency..."
	./$(TARGET_FCNTL) "Msg 1 from fcntl" & \
	./$(TARGET_FCNTL) "Msg 2 from fcntl" & \
	./$(TARGET_FCNTL) "Msg 3 from fcntl" & \
	./$(TARGET_FCNTL) "Msg 4 from fcntl" & \
	./$(TARGET_FCNTL) "Msg 5 from fcntl" & \
	wait
	@echo "--- Resulting system.log (fcntl) ---"
	@cat system.log

clean:
	rm -f $(TARGET_FLOCK) $(TARGET_FCNTL) system.log