CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu99 -g

all: collector sensor

collector: receiver.c
	$(CC) $(CFLAGS) -o collector receiver.c

sensor: sender.c
	$(CC) $(CFLAGS) -o sensor sender.c

clean:
	rm -f collector sensor

.PHONY: all clean