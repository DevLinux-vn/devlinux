# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Iinclude

# Linker flags
LDFLAGS = -Llib

# Libraries
LDLIBS = -llogger -lcalc -lm

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

# Output
TARGET = $(BIN_DIR)/main

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Library objects
CALC_OBJ = $(OBJ_DIR)/calc.o
LOGGER_OBJ = $(OBJ_DIR)/logger.o
MAIN_OBJ = $(OBJ_DIR)/main.o

# Libraries
STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so

# Default target
.PHONY: all
all: $(TARGET)

# Build all libraries
.PHONY: libs
libs: static shared

# Build static library
.PHONY: static
static: $(STATIC_LIB)

$(STATIC_LIB): $(CALC_OBJ)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $^

# Build shared library
.PHONY: shared
shared: $(SHARED_LIB)

$(SHARED_LIB): $(LOGGER_OBJ)
	@mkdir -p $(LIB_DIR)
	$(CC) -shared -o $@ $^

# Pattern rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# Build executable
$(TARGET): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB)
	@mkdir -p $(BIN_DIR)
	$(CC) $(MAIN_OBJ) $(STATIC_LIB) -L$(LIB_DIR) -llogger -Wl,-rpath,'$$ORIGIN/../lib' -o $@ -lm

# Run application
.PHONY: run
run: $(TARGET)
	./$(TARGET)

# Clean
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

# Rebuild
.PHONY: rebuild
rebuild: clean all
