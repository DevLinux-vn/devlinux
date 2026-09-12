CC = gcc
CFLAGS = -Wall -Iinclude -fPIC

BUILD_DIR = build
BIN_DIR = bin

OBJ = $(BUILD_DIR)/stringutils.o
LIB = $(BIN_DIR)/libstringutils.so
TARGET = $(BIN_DIR)/main

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ): src/stringutils.c include/stringutils.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c src/stringutils.c -o $(OBJ)

$(LIB): $(OBJ) | $(BIN_DIR)
	$(CC) -shared -o $(LIB) $(OBJ)

$(TARGET): main.c $(LIB)
	$(CC) main.c -Iinclude -L$(BIN_DIR) -lstringutils -o $(TARGET)

run: $(TARGET)
	LD_LIBRARY_PATH=$(BIN_DIR) ./$(TARGET) Embedded

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)