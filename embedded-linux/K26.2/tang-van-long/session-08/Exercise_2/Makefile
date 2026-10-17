CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g

all: sensor collector

sensor: sender.c
	$(CC) $(CFLAGS) -o sensor sender.c

collector: receiver.c
	$(CC) $(CFLAGS) -o collector receiver.c

clean:
	rm -f sensor collector

.PHONY: all clean