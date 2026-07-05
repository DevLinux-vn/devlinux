CC=gcc
CFLAGS=-Wall -Wextra -std=c11

all: sensor-daemon sensor-cli

sensor-daemon: daemon.c sensor_shm.h
	$(CC) $(CFLAGS) daemon.c -o sensor-daemon

sensor-cli: cli.c sensor_shm.h
	$(CC) $(CFLAGS) cli.c -o sensor-cli

clean:
	rm -f sensor-daemon sensor-cli