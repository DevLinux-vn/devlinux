CC = gcc

CFLAGS = -Wall -Wextra -Iinclude -fPIC

BUILD_DIR = build
BIN_DIR = bin

LIB_NAME = $(BIN_DIR)/libstringutils.so

APP = $(BIN_DIR)/main

LIB_OBJ = $(BUILD_DIR)/stringutils.o
MAIN_OBJ = $(BUILD_DIR)/main.o

all: $(APP)

# Create shared library
$(LIB_NAME): $(LIB_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) -shared -o $(LIB_NAME) $(LIB_OBJ)

# Build executable
$(APP): $(MAIN_OBJ) $(LIB_NAME)
	$(CC) $(MAIN_OBJ) -L$(BIN_DIR) -lstringutils \
	-Wl,-rpath,'$$ORIGIN' \
	-o $(APP)

# Compile library source
$(LIB_OBJ): src/stringutils.c include/stringutils.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c src/stringutils.c -o $(LIB_OBJ)

# Compile main source
$(MAIN_OBJ): src/main.c include/stringutils.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c src/main.c -o $(MAIN_OBJ)

run: all
	./$(APP) "hello_linux"

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean run