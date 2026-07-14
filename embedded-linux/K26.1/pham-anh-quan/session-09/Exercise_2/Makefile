CC = gcc
CFLAGS = -Wall -Wextra -g

all: writer reader

writer: writer.o
		$(CC) $(CFLAGS) -o writer writer.o

reader: reader.o
		$(CC) $(CFLAGS) -o reader reader.o

writer.o: writer.c device_cfg.h
		$(CC) $(CFLAGS) -c writer.c

reader.o: reader.c device_cfg.h
		$(CC) $(CFLAGS) -c reader.c

clean:
		rm -f *.o writer readerS