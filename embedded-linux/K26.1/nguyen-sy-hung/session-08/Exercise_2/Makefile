CC := gcc

CFLAGS := -Wall -Wextra -Werror -std=c11

SENDER_TARGET := sensor
RECEIVER_TARGET := collector

SENDER_SRC := sender.c
RECEIVER_SRC := receiver.c

TARGETS := $(SENDER_TARGET) $(RECEIVER_TARGET)

.PHONY: all clean

all: $(TARGETS)

$(SENDER_TARGET): $(SENDER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(RECEIVER_TARGET): $(RECEIVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)