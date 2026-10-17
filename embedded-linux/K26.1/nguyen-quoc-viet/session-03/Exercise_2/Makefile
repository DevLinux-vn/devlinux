CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = product_manager

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET) products.dat

.PHONY: all clean
