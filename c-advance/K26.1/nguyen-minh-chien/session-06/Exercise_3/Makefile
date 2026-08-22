CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99
TARGET = dispatcher
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)