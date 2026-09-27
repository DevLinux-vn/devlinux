CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99
TARGET = sensor_daemon

# Targets required: all, clean
all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: all clean

