CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99
TARGET = sensor_app
SRCS = main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean