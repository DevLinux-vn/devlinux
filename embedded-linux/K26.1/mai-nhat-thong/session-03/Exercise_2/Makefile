CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=gnu99 -O2 -g
TARGET = main

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -f $(TARGET) products.dat