CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread

TARGET = print_queue

all: $(TARGET)

$(TARGET): print_queue.c
	$(CC) $(CFLAGS) -o $(TARGET) print_queue.c

clean:
	rm -f $(TARGET)