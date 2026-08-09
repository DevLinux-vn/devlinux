CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -I$(INC_DIR) -std=c99

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
LIB_DIR = lib
APP_DIR = app

# Output files
TARGET = main
LIB = $(LIB_DIR)/libmathutils.a

# Source files
SRC = $(SRC_DIR)/mathutils.c
OBJ = $(OBJ_DIR)/mathutils.o
APP_SRC = $(APP_DIR)/main.c

.PHONY: all run clean

all: $(TARGET)

# Compile source file into object file
$(OBJ): $(SRC) $(INC_DIR)/mathutils.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create static library
$(LIB): $(OBJ)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $^

# Build application
$(TARGET): $(APP_SRC) $(LIB)
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lmathutils -o $@

# Run application
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(LIB)
	rm -f $(TARGET)
	
