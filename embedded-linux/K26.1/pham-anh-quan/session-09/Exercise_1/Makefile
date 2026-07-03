CC = gcc
CFLAGS = -Wall -Wextra -g

all: daemon cli

daemon: daemon.o
		$(CC) $(CFLAGS) -o daemon daemon.o

cli: cli.o
		$(CC) $(CFLAGS) -o cli cli.o

daemon.o: daemon.c sensor_shm.h
		$(CC) $(CFLAGS) -c daemon.c

cli.o: cli.c sensor_shm.h
		$(CC) $(CFLAGS) -c cli.c

clean:
		rm -f *.o daemon cli	