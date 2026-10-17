CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -Werror -std=c99
TARGET  = compute_stats
SRC     = main.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)