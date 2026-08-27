CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread

TARGET = booking

all: $(TARGET)

$(TARGET): booking.c
	$(CC) $(CFLAGS) booking.c -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean