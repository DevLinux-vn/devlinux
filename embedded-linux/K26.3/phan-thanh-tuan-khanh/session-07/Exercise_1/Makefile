CC=gcc
CFLAGS=-Wall -Wextra -std=c11
TARGET=sensor_daemon

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)