CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread

ROOT = $(CURDIR)
SRC = $(ROOT)/src
BIN = $(ROOT)/bin

all: $(BIN)/agent $(BIN)/server

$(BIN):
	mkdir -p $(BIN)

$(BIN)/agent: $(SRC)/agent/main.c $(SRC)/agent/agent_id.c $(SRC)/agent/metrics.c $(SRC)/agent/dashboard.c $(SRC)/server/common.c $(SRC)/server/logger.c $(SRC)/server/keepalive.c $(SRC)/server/config.c | $(BIN)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN)/server: $(SRC)/server/main.c $(SRC)/server/common.c $(SRC)/server/dashboard.c $(SRC)/server/logger.c $(SRC)/server/keepalive.c $(SRC)/server/command.c $(SRC)/server/config.c | $(BIN)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(BIN) logs

.PHONY: all clean
