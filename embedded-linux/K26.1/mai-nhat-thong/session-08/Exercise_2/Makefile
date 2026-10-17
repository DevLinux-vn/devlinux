CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -std=gnu99
TARGETS = receiver sender

all: $(TARGETS)

receiver: receiver.c
	$(CC) $(CFLAGS) -o $@ $<

sender: sender.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean