CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200809L

SERVER=monitor-daemon
CLIENT=monitor-cli

all: $(SERVER) $(CLIENT)

$(SERVER): server.c
	$(CC) $(CFLAGS) -o $(SERVER) server.c

$(CLIENT): client.c
	$(CC) $(CFLAGS) -o $(CLIENT) client.c

clean:
	rm -f $(SERVER) $(CLIENT)
	rm -f /tmp/monitor.sock