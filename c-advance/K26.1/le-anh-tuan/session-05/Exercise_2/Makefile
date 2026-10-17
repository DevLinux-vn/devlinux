CC := gcc
TARGET := hw_jump_table
SOURCE := hw_jump_table.c

CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)

clean:
	rm -f $(TARGET)