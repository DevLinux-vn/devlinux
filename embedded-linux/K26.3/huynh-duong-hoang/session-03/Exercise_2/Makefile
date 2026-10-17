CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = product_mgr

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET) products.dat

.PHONY: all clean
