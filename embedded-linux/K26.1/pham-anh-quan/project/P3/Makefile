CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -D_GNU_SOURCE
LIBS = -lcrypt

SERVER_DIR = src/server
CLIENT_DIR = src/client

SERVER_SRCS = $(SERVER_DIR)/main.c $(SERVER_DIR)/auth.c $(SERVER_DIR)/broadcast.c $(SERVER_DIR)/history.c
CLIENT_SRCS = $(CLIENT_DIR)/main.c

SERVER_BIN = chat_server
CLIENT_BIN = chat_client

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(CLIENT_BIN): $(CLIENT_SRCS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN) users.db chat_history.txt

.PHONY: all clean