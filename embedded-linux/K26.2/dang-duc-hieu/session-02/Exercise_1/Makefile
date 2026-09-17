CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
AR = ar
ARFLAGS = rcs

# Directories
SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
APP_DIR = app

# Target executable and static library
TARGET = main
LIBRARY = $(LIB_DIR)/libmathutils.a

# Source and Object Files
LIB_SRC = $(SRC_DIR)/mathutils.c
LIB_OBJ = $(OBJ_DIR)/mathutils.o
APP_SRC = $(APP_DIR)/main.c

.PHONY: all clean

all: $(TARGET)

# Create object and library directories automatically
$(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

# Compile mathutils.c to an object file
$(LIB_OBJ): $(LIB_SRC) include/mathutils.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Package object file into a static library
$(LIBRARY): $(LIB_OBJ) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $<

# Build the final application executable
$(TARGET): $(APP_SRC) $(LIBRARY)
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lmathutils -o $@

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(TARGET)