CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = product_manager
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) products.dat