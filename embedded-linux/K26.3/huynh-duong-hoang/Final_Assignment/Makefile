CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude -DDEBUG
TARGET = server

# Chế độ debug (bật cờ -DDEBUG qua tiền xử lý)
debug: CFLAGS += -DDEBUG
debug: clean $(TARGET)

all: $(TARGET)

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET) gateway.log logFifo

.PHONY: all debug clean