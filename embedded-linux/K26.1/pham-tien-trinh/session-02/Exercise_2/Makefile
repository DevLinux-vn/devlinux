CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -fPIC

BUILD_DIR = build
BIN_DIR = bin

LIB = $(BIN_DIR)/libstringutils.so
TARGET = $(BIN_DIR)/main

all: $(TARGET)

$(TARGET): main.c $(LIB)
	mkdir -p $(BIN_DIR)
	$(CC) -Wall -Wextra -Iinclude main.c -L$(BIN_DIR) -lstringutils -Wl,-rpath,$(BIN_DIR) -o $(TARGET)

$(LIB): $(BUILD_DIR)/stringutils.o
	mkdir -p $(BIN_DIR)
	$(CC) -shared -o $(LIB) $(BUILD_DIR)/stringutils.o

$(BUILD_DIR)/stringutils.o: src/stringutils.c include/stringutils.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c src/stringutils.c -o $(BUILD_DIR)/stringutils.o

run: $(TARGET)
	./$(TARGET) hello

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all run clean