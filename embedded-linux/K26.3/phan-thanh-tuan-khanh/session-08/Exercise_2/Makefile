CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200809L

SENDER=sensor
RECEIVER=collector

all: $(SENDER) $(RECEIVER)

$(SENDER): sender.c
	$(CC) $(CFLAGS) -o $(SENDER) sender.c

$(RECEIVER): receiver.c
	$(CC) $(CFLAGS) -o $(RECEIVER) receiver.c

clean:
	rm -f $(SENDER) $(RECEIVER)