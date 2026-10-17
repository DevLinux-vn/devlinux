CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = student_manager

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET) students.dat

.PHONY: all clean
