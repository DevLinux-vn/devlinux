CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
TARGETS = writer reader

all: $(TARGETS)

writer: writer.c
	$(CC) $(CFLAGS) -o $@ $<

reader: reader.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean