CC = gcc

CFLAGS = -Wall -Wextra -pthread -std=c99

TARGET = print_queue
SRC = print_queue.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

