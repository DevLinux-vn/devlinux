CC = gcc
CFLAGS = -Wall -Wextra

TARGET = main

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -f $(TARGET) products.dat

.PHONY: all clean