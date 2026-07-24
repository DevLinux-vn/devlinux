CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99
TARGET = hw_array_of_pointers
SRC = hw_array_of_pointers.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)