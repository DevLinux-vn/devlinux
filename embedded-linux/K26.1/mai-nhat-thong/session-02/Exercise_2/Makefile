CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -std=gnu99 -O2 -Iinclude

BUILD_DIR = build
BIN_DIR = bin

SRC = src/stringutils.c
OBJ = $(BUILD_DIR)/stringutils.o
SHARED_LIB = $(BIN_DIR)/libstringutils.so
TARGET = $(BIN_DIR)/main

SAMPLE_STR = "Embedded_Linux_2026"

.PHONY: all clean run

all: $(TARGET)

$(OBJ): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(SHARED_LIB): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) -shared -o $@ $<

$(TARGET): main.c $(SHARED_LIB)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -L$(BIN_DIR) -lstringutils -Wl,-rpath,'$$ORIGIN' -o $@

run: $(TARGET)
	./$(TARGET) $(SAMPLE_STR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)