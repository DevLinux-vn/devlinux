CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
PICFLAGS = -fPIC
LDFLAGS = -Lbin -lstringutils -Wl,-rpath,./bin

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Default string argument if none is provided
ARGS ?= "Hello World"

# Targets
TARGET = $(BIN_DIR)/main
LIB_OBJ = $(BUILD_DIR)/stringutils.o
SHARED_LIB = $(BIN_DIR)/libstringutils.so

.PHONY: all clean verify run

all: $(TARGET)

# Automatically create output directories
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Compile stringutils.c with -fPIC for shared library support
$(LIB_OBJ): $(SRC_DIR)/stringutils.c include/stringutils.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(PICFLAGS) -c $< -o $@

# Build the dynamic shared library (.so)
$(SHARED_LIB): $(LIB_OBJ) | $(BIN_DIR)
	$(CC) -shared -o $@ $<

# Compile and link main executable against the shared library
$(TARGET): main.c $(SHARED_LIB)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

# Execute the binary with an argument
run: $(TARGET)
	./$(TARGET) $(ARGS)

# Check shared library dependencies
verify: $(TARGET)
	ldd $(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)