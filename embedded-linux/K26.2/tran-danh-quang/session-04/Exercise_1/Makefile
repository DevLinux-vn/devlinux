CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

TARGET = manager
SRCS = manager.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
