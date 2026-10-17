CC = gcc
CFLAGS = -Wall -pthread
TARGET = booking

all: $(TARGET)

$(TARGET): booking.c
	$(CC) $(CFLAGS) -o $(TARGET) booking.c

clean:
	rm -f $(TARGET)

.PHONY: all clean
