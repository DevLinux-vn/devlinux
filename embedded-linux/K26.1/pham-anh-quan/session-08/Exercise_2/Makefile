CC = gcc
CFLAGS = -Wall -Wextra -g

all: collector sensor

collector: receiver.o
	$(CC) $(CFLAGS) -o collector receiver.o

sensor: sender.o
	$(CC) $(CFLAGS) -o sensor sender.o

receiver.o: receiver.c
	$(CC) $(CFLAGS) -c receiver.c

sender.o: sender.c
	$(CC) $(CFLAGS) -c sender.c

clean:
	rm -f *.o collector sensor