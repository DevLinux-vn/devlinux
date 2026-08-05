CC = gcc

CFLAGS = -Wall -Wextra -pthread

TARGET = print_queue

SRC = print_queue.c


all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
	./$(TARGET)


clean:
	rm -f $(TARGET)