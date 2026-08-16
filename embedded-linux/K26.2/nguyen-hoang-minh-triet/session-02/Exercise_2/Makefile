# Define compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -I$(INC_DIR)

# Directories
APP_DIR = app
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
BUILD_DIR = build
LIB_DIR = $(BIN_DIR)
OBJ_DIR = $(BUILD_DIR)

# Output files
TARGET = $(BIN_DIR)/main
LIB = $(LIB_DIR)/libstringutils.so

# Source files
SRC = $(SRC_DIR)/stringutils.c
BUILD = $(OBJ_DIR)/stringutils.o
APP = $(APP_DIR)/main.c

.PHONY: all run clean

# Build everything
all: $(TARGET)

# Build executable
$(TARGET): $(APP) $(LIB)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(APP) -L$(LIB_DIR) -lstringutils -o $(TARGET)

# Build shared library
$(LIB): $(BUILD)
	@mkdir -p $(LIB_DIR)
	$(CC) -shared -o $(LIB) $(BUILD)

# Compile source file into object file
$(BUILD): $(SRC) $(INC_DIR)/stringutils.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -fPIC -c $(SRC) -o $(BUILD)

# Run the program
run: $(TARGET)
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(TARGET) "Hello World"

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)