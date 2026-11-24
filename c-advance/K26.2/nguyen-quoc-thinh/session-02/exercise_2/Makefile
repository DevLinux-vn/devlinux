CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -O0
TARGET = stack_monitor

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)

.PHONY: all clean