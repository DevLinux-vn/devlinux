CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -Iinc

SRC = src/*.c test/*.c 

BUILD_DIR = build

TARGET = $(BUILD_DIR)/output

all: $(TARGET)

$(TARGET): $(BUILD_DIR) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)