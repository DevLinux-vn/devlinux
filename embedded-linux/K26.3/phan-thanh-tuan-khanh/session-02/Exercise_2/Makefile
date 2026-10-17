CC = gcc

CFLAGS = -Wall -Wextra -Iinclude
PICFLAGS = -fPIC

BUILD_DIR = build
BIN_DIR = bin

LIB = $(BIN_DIR)/libstringutils.so
OBJ = $(BUILD_DIR)/stringutils.o
TARGET = $(BIN_DIR)/main

.PHONY: all clean run

all: $(TARGET)

$(TARGET): main.c $(LIB)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) main.c -L$(BIN_DIR) -lstringutils -Wl,-rpath,'$$ORIGIN' -o $(TARGET)

$(LIB): $(OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) -shared -o $(LIB) $(OBJ)

$(OBJ): src/stringutils.c include/stringutils.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(PICFLAGS) -c src/stringutils.c -o $(OBJ)

run: $(TARGET)
	./$(TARGET) "Hello Linux"

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)