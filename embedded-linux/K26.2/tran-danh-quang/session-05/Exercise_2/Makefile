CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -pthread

TARGET = print_queue
SRCS = print_queue.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
