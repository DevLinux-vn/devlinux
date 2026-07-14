TARGET = transaction_logger
CC = gcc
CFLAGS = -Wall -Wextra

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)

.PHONY: all clean