CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -Werror -pedantic

TARGET := exercise_5
SRC := main.c

.PHONY: all clean

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)