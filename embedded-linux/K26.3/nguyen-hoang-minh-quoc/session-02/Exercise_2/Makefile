CC := gcc
CFLAGS := -Wall -Wextra -Iinclude
PICFLAGS := -fPIC
LDFLAGS := -Lbin -Wl,-rpath,'$$ORIGIN'
LDLIBS := -lstringutils

BUILD_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/main
LIB := $(BIN_DIR)/libstringutils.so

all: $(TARGET)

$(TARGET): src/main.c $(LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

$(LIB): $(BUILD_DIR)/stringutils.o | $(BIN_DIR)
	$(CC) -shared -o $@ $<

$(BUILD_DIR)/stringutils.o: src/stringutils.c include/stringutils.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(PICFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

run: all
	./$(TARGET) "Hello Embedded Linux"

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all run clean
