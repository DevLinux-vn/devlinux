CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99

TARGET = product_manager
SRC = main.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) products.dat
	