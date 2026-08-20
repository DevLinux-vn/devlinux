CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := exercise_1
SRC := main.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)