CC = gcc
CFLAGS = -Wall -g
TARGET = product_manager

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)
clean:
	rm -f $(TARGET)