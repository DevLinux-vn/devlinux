CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

TARGET = stack_monitor

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -f $(TARGET)