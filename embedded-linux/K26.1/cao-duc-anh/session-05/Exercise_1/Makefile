CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -std=gnu11 -pthread
TARGET = booking
SRC = booking.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
