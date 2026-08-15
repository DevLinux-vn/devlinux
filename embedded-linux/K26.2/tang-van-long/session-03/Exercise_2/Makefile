CC := gcc

CFLAGS := -Wall -Wextra -Werror -std=c99 -O2 -g

TARGET := main

.PHONY: all clean run

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) products.dat