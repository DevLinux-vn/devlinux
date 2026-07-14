CC = gcc
CFLAGS = -Wall -Wextra -g

all: monitor-daemon monitor-cli

monitor-daemon: server.o
	$(CC) $(CFLAGS) -o monitor-daemon server.o

monitor-cli: client.o
	$(CC) $(CFLAGS) -o monitor-cli client.o

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

clean:
	rm -f *.o monitor-daemon monitor-cli /tmp/monitor.sock