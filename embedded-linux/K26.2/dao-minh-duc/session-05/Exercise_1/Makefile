CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -pthread

TARGET = booking

.PHONY: all clean

all: $(TARGET)

$(TARGET): booking.c
	$(CC) $(CFLAGS) -o $(TARGET) booking.c

clean:
	rm -f $(TARGET)
