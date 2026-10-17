CC := gcc

CFLAGS := -Wall -Wextra -Werror -std=c11

SERVER_TARGET := monitor-daemon
CLIENT_TARGET := monitor-cli

SERVER_SRC := server.c
CLIENT_SRC := client.c

TARGETS := $(SERVER_TARGET) $(CLIENT_TARGET)

.PHONY: all clean

all: $(TARGETS)

$(SERVER_TARGET): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT_TARGET): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
	rm -f /tmp/monitor.sock