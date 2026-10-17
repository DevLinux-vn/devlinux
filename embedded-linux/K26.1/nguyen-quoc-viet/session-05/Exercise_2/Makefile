CC      = gcc
CFLAGS  = -Wall -Wextra -pthread
TARGET  = print_queue

all: $(TARGET)

$(TARGET): print_queue.c
	$(CC) $(CFLAGS) -o $(TARGET) print_queue.c

clean:
	rm -f $(TARGET)
