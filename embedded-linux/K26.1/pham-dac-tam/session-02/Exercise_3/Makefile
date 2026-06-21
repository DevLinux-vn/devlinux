CC = gcc
CFLAGS = -I./include -Wall -Wextra -fPIC
LDFLAGS = -L./lib
LDLIBS = -lcalc -llogger

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

# Source files (automatic wildcard)
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Library components
CALC_OBJ = $(OBJ_DIR)/calc.o
LOGGER_OBJ = $(OBJ_DIR)/logger.o
MAIN_OBJ = $(OBJ_DIR)/main.o

STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so

EXECUTABLE = $(BIN_DIR)/app

# Phony targets
.PHONY: all libs static shared run clean rebuild

# Default target
all: $(EXECUTABLE)

# Build all libraries
libs: $(STATIC_LIB) $(SHARED_LIB)

# Build static library only
static: $(STATIC_LIB)

# Build shared library only
shared: $(SHARED_LIB)

# Build static library
$(STATIC_LIB): $(CALC_OBJ)
	mkdir -p $(LIB_DIR)
	ar rcs $@ $^

# Build shared library
$(SHARED_LIB): $(LOGGER_OBJ)
	mkdir -p $(LIB_DIR)
	$(CC) -shared -o $@ $^

# Build executable
$(EXECUTABLE): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) $(LDFLAGS) $(LDLIBS)

# Pattern rule for object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the executable
run: $(EXECUTABLE)
	LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH $(EXECUTABLE)
	@echo ""
	@echo "=== Contents of app.log ===" 
	@cat app.log || echo "No log file yet"

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

# Rebuild from scratch
rebuild: clean all
