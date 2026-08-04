CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=gnu99 -O2 -g

TARGET = monitor
SRCS = monitor.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
