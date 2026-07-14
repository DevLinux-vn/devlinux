CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu99 -g

TARGET = monitor
SRCS = monitor.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean