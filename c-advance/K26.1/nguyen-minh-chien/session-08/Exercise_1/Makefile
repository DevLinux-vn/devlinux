CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -Werror -std=c99
TARGET  = main
SRC     = main.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)