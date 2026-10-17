# Makefile - Exercise_2: shared library libstringutils.so
# Targets: all, run, clean

CC       := gcc
CFLAGS   := -Wall -Wextra -Iinclude
SAMPLE   := hello

BUILD    := build
BIN      := bin

LIB_NAME := stringutils
LIB      := $(BIN)/lib$(LIB_NAME).so
OBJ      := $(BUILD)/stringutils.o
EXE      := $(BIN)/main

# Default target: build everything
all: $(EXE)

# 1) Compile library source with -fPIC (Position Independent Code)
$(OBJ): src/stringutils.c include/stringutils.h | $(BUILD)
	$(CC) $(CFLAGS) -fPIC -c src/stringutils.c -o $(OBJ)

# 2) Link object into a shared library (.so)
$(LIB): $(OBJ) | $(BIN)
	$(CC) -shared -o $(LIB) $(OBJ)

# 3) Compile main and dynamically link against libstringutils.so
$(EXE): main.c $(LIB) | $(BIN)
	$(CC) $(CFLAGS) main.c -L$(BIN) -l$(LIB_NAME) -o $(EXE)

# Create output directories if missing
$(BUILD):
	mkdir -p $(BUILD)
$(BIN):
	mkdir -p $(BIN)

# Run the program on a sample string.
# LD_LIBRARY_PATH tells the loader where to find libstringutils.so at runtime.
run: $(EXE)
	LD_LIBRARY_PATH=$(BIN) $(EXE) $(SAMPLE)

# Remove all build artifacts
clean:
	rm -rf $(BUILD) $(BIN)

.PHONY: all run clean
