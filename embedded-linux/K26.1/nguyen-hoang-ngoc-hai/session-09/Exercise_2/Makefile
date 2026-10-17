CC=gcc
CFLAGS=-Wall -Wextra -Werror -g -std=gnu99

all: config-writer config-reader

config-writer: writer.c device_cfg.h
	$(CC) $(CFLAGS) writer.c -o config-writer

config-reader: reader.c device_cfg.h
	$(CC) $(CFLAGS) reader.c -o config-reader

clean:
	rm -f config-writer config-reader