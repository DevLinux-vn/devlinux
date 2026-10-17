CC = gcc
CFLAGS = -Wall -Wextra -std=c11

SENDER = sensor
RECEIVER = collector

all: $(SENDER) $(RECEIVER)

$(SENDER): sender.c
	$(CC) $(CFLAGS) sender.c -o $(SENDER)

$(RECEIVER): receiver.c
	$(CC) $(CFLAGS) receiver.c -o $(RECEIVER)

clean:
	rm -f $(SENDER) $(RECEIVER)
