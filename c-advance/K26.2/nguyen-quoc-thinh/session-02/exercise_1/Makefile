CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -rdynamic
LDLIBS = -ldl
TARGET = memory_map

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean