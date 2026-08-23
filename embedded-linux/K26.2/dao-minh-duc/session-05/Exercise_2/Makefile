CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -pthread

TARGET = print_queue

all: $(TARGET)

$(TARGET): print_queue.c
	$(CC) $(CFLAGS) print_queue.c -o $(TARGET)

clean:
	rm -f $(TARGET)
