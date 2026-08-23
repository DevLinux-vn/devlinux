CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99
TARGET = display_app
SRCS = main.c console_display.c dummy_display.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean