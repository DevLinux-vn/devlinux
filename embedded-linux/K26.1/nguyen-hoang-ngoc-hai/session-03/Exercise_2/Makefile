CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = product_manager

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean