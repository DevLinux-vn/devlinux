CC      = gcc
CFLAGS  = -Wall -Wextra -O2
TARGETS = sensor-daemon sensor-cli

all: $(TARGETS)

sensor-daemon: daemon.c sensor_shm.h
	$(CC) $(CFLAGS) -o $@ daemon.c

sensor-cli: cli.c sensor_shm.h
	$(CC) $(CFLAGS) -o $@ cli.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
