CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -Iinc

TARGET = output 

SRC = src/main.c src/mac_parser.c 

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean: 
	rm -rf $(TARGET)