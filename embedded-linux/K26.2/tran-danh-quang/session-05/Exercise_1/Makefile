CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -pthread

TARGET = booking
SRCS = booking.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
