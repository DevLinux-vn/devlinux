CC=gcc
CFLAGS=-Wall -Wextra -std=c11
TARGET=transaction_logger

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)