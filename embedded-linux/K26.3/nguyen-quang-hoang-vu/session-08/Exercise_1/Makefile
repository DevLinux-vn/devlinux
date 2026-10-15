CC=gcc
CFLAGS=-Wall -Wextra

all: server client

server:
	$(CC) $(CFLAGS) server.c -o server

client:
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f server client
