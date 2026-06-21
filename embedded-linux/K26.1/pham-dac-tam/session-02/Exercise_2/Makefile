CC = gcc
CFLAGS = -I./include -Wall -Wextra -fPIC
LDFLAGS = -L./bin -lstringutils

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Files
LIB_SOURCES = $(SRC_DIR)/stringutils.c
LIB_OBJECTS = $(BUILD_DIR)/stringutils.o
SHARED_LIB = $(BIN_DIR)/libstringutils.so

APP_SOURCES = $(SRC_DIR)/main.c
APP_OBJECTS = $(BUILD_DIR)/main.o
EXECUTABLE = $(BIN_DIR)/app

# Phony targets
.PHONY: all clean run

# Default target
all: $(SHARED_LIB) $(EXECUTABLE)

# Build shared library
$(SHARED_LIB): $(LIB_OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CC) -shared -o $@ $^

# Build executable
$(EXECUTABLE): $(APP_OBJECTS) $(SHARED_LIB)
	$(CC) $(CFLAGS) -o $@ $(APP_OBJECTS) $(LDFLAGS)

# Compile .c files to .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the executable
run: all
	LD_LIBRARY_PATH=$(BIN_DIR):$$LD_LIBRARY_PATH $(EXECUTABLE) "HelloLinux"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
