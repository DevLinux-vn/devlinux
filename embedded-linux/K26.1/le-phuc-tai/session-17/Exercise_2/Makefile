CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -std=gnu99 -g
TARGET  = led_app
SRCS    = main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean