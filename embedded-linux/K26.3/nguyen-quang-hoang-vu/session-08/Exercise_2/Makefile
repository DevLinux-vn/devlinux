CC=gcc
CFLAGS=-Wall -Wextra

all: sender receiver

sender:
	$(CC) $(CFLAGS) sender.c -o sender

receiver:
	$(CC) $(CFLAGS) receiver.c -o receiver

clean:
	rm -f sender receiver
