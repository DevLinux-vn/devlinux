CC = gcc
CFLAGS = -Wall -Wextra -std=c11

SERVER = monitor-daemon
CLIENT = monitor-cli

all: $(SERVER) $(CLIENT)

$(SERVER): server.c
	$(CC) $(CFLAGS) server.c -o $(SERVER)

$(CLIENT): client.c
	$(CC) $(CFLAGS) client.c -o $(CLIENT)

clean:
	rm -f $(SERVER) $(CLIENT)
	rm -f /tmp/monitor.sock
