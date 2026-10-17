CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = product_manager

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET) products.dat