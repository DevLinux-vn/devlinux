CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -g
TARGET  := gateway

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)

.PHONY: all clean