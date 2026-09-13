CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

TARGETS = sensor collector

.PHONY: all clean

all: $(TARGETS)

sensor: sender.c
	$(CC) $(CFLAGS) sender.c -o sensor

collector: receiver.c
	$(CC) $(CFLAGS) receiver.c -o collector

clean:
	rm -f $(TARGETS)
