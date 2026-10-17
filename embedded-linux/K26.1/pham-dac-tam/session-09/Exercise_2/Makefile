CC = gcc
CFLAGS = -Wall -Wextra -g

TARGETS = writer reader

all: $(TARGETS)

writer: writer.c
	$(CC) $(CFLAGS) -o writer writer.c

reader: reader.c
	$(CC) $(CFLAGS) -o reader reader.c

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
