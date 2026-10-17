CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -pthread
TARGET = print_queue
SRC = print_queue.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean