CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
PICFLAGS = -fPIC
AR = ar
ARFLAGS = rcs

# Directories
SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

# Targets
TARGET = $(BIN_DIR)/main
STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so

# Source files using wildcard
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Object files generated via pattern replacement
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all clean verify run

all: $(TARGET)

# Directory generation rules
$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	mkdir -p $@

# Generic pattern rule to compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(PICFLAGS) -c $< -o $@

# Build static library (libcalc.a)
$(STATIC_LIB): $(OBJ_DIR)/calc.o | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $<

# Build shared library (liblogger.so)
$(SHARED_LIB): $(OBJ_DIR)/logger.o | $(LIB_DIR)
	$(CC) -shared -o $@ $<

# Compile and link main executable against both libraries
$(TARGET): main.c $(STATIC_LIB) $(SHARED_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lcalc -llogger -Wl,-rpath,./$(LIB_DIR) -o $@

# Execute target binary
run: $(TARGET)
	./$(TARGET)

# Verify dependency linkage
verify: $(TARGET)
	ldd $(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)
