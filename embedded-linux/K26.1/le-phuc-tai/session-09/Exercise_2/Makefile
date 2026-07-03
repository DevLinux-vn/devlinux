CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu99 -g

all: config-writer config-reader

config-writer: writer.c
	$(CC) $(CFLAGS) -o config-writer writer.c

config-reader: reader.c
	$(CC) $(CFLAGS) -o config-reader reader.c

clean:
	rm -f config-writer config-reader

.PHONY: all clean