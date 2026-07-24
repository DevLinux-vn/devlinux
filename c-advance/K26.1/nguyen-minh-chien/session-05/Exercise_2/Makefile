CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99
TARGET = hw_jump_table
SRCS = hw_jump_table.c

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)