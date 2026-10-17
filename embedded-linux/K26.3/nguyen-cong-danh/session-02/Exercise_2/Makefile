CC = gcc

CFLAGS = -Wall -Wextra -fPIC -Iinclude

BUILD_DIR = build
BIN_DIR = bin

TARGET_LIB = $(BIN_DIR)/libstringutils.so
TARGET_APP = $(BIN_DIR)/main

all: $(TARGET_LIB) $(TARGET_APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR)/stringutils.o: src/stringutils.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_LIB): $(BUILD_DIR)/stringutils.o | $(BIN_DIR)
	$(CC) -shared -o $@ $^

$(TARGET_APP): main.c $(TARGET_LIB)
	$(CC) main.c -Iinclude -L$(BIN_DIR) -lstringutils \
	-Wl,-rpath='$$ORIGIN' -o $@

run: all
	./$(TARGET_APP) "hello linux"

clean:
	rm -rf build bin

.PHONY: all run clean