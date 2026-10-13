CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread

TARGET = booking

all: $(TARGET)

$(TARGET): booking.c
	$(CC) $(CFLAGS) -o $(TARGET) booking.c

clean:
	rm -f $(TARGET)