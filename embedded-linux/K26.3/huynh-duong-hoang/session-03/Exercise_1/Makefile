CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = student_mgr

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET) students.dat

.PHONY: all clean
