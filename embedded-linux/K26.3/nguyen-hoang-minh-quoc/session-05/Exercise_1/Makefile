CC = gcc
CFLAGS = -Wall -Wextra -pthread -O2
TARGET = booking
SRC = booking.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
