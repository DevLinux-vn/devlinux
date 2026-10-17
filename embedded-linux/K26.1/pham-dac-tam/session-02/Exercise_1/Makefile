CC = gcc
CFLAGS = -I./include -Wall -Wextra
AR = ar

# Directories
SRC_DIR = src
APP_DIR = app
INC_DIR = include
OBJ_DIR = obj
LIB_DIR = lib

# Files
LIB_SOURCES = $(SRC_DIR)/mathutils.c
LIB_OBJECTS = $(OBJ_DIR)/mathutils.o
STATIC_LIB = $(LIB_DIR)/libmathutils.a

APP_SOURCES = $(APP_DIR)/main.c
APP_OBJECTS = $(OBJ_DIR)/main.o
EXECUTABLE = main

# Phony targets
.PHONY: all clean run

# Default target
all: $(EXECUTABLE)

# Build executable
$(EXECUTABLE): $(APP_OBJECTS) $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $^ -L$(LIB_DIR) -lmathutils

# Build static library
$(STATIC_LIB): $(LIB_OBJECTS)
	mkdir -p $(LIB_DIR)
	$(AR) rcs $@ $^

# Compile .c files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(APP_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the executable
run: $(EXECUTABLE)
	echo "5\n3" | ./$(EXECUTABLE)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(EXECUTABLE)

