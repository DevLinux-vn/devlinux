CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGETS = sensor collector

all: $(TARGETS)

sensor: sender.c
	$(CC) $(CFLAGS) sender.c -o sensor

collector: receiver.c
	$(CC) $(CFLAGS) receiver.c -o collector

clean:
	rm -f $(TARGETS)
