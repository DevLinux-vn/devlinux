CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = main

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) *.dat
