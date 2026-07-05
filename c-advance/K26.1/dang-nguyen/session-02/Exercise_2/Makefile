CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := stack_monitor
SRC := main.c
INC := -I.

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INC) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)