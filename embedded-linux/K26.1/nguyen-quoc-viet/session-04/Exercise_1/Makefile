CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11

TARGET  = manager

.PHONY: all clean

all: $(TARGET)

$(TARGET): manager.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
