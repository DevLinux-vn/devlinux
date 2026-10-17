CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = manager

all: $(TARGET)

$(TARGET): manager.c
	$(CC) $(CFLAGS) -o $(TARGET) manager.c

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
