CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -Werror -std=c99
TARGET  = Exercise_1
SRC     = main.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)