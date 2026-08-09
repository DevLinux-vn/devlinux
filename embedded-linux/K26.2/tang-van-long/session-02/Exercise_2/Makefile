CUR_DIR :=.
INC_DIR := $(CUR_DIR)/include
SRC_DIR := $(CUR_DIR)/src
BIN_DIR := $(CUR_DIR)/bin
BUILD_DIR := $(CUR_DIR)/build

LIB_NAME := stringutils
CC := gcc
CFLAGS := -I $(INC_DIR)

all: main

make_dirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(BUILD_DIR)

stringutils.o: $(SRC_DIR)/stringutils.c make_dirs
	$(CC) -fPIC -c $(SRC_DIR)/stringutils.c -o $(BUILD_DIR)/stringutils.o $(CFLAGS)

libstringutils.so: stringutils.o
	$(CC) -shared -o $(BIN_DIR)/libstringutils.so $(BUILD_DIR)/stringutils.o

main: libstringutils.so
	$(CC) main.c $(CFLAGS) -L$(BIN_DIR) -l$(LIB_NAME) -Wl,-rpath,'./bin' -o $(BIN_DIR)/main

run:
	LD_LIBRARY_PATH=$(BIN_DIR) $(BIN_DIR)/main hello
	
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)