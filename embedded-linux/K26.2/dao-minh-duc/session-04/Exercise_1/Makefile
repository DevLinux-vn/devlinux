CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

TARGET = manager

.PHONY: all clean

all: $(TARGET)

$(TARGET): manager.c
	$(CC) $(CFLAGS) -o $(TARGET) manager.c

clean:
	rm -f $(TARGET)
