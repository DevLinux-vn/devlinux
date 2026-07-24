CC := gcc

CFLAGS := -Wall -Wextra -Werror -std=c11

DAEMON_TARGET := sensor-daemon
CLI_TARGET := sensor-cli

DAEMON_SRC := daemon.c
CLI_SRC := cli.c

TARGETS := $(DAEMON_TARGET) $(CLI_TARGET)

.PHONY: all clean

all: $(TARGETS)

$(DAEMON_TARGET): $(DAEMON_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(CLI_TARGET): $(CLI_SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
	ipcrm -M 0x1234 2>/dev/null || true