# Compiler settings
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -pedantic -std=c99 -O2

# Targets
TARGETS := receiver sender

.PHONY: all clean

all: $(TARGETS)

receiver: receiver.c
	$(CC) $(CFLAGS) -o $@ $<

sender: sender.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) *.o

	