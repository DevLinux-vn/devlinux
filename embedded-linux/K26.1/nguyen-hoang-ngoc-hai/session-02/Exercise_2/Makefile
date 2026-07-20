CC = gcc

CFLAGS = -Wall -Wextra -Iinclude -fPIC -Werror -std=gnu99

BUILD_DIR = build
BIN_DIR = bin

OBJ = $(BUILD_DIR)/stringutils.o
LIB = $(BIN_DIR)/libstringutils.so
APP = $(BIN_DIR)/main

all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ): src/stringutils.c include/stringutils.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c src/stringutils.c -o $(OBJ)

$(LIB): $(OBJ) | $(BIN_DIR)
	$(CC) -shared -o $(LIB) $(OBJ)

$(APP): main.c $(LIB)
	$(CC) main.c -Iinclude -L$(BIN_DIR) -lstringutils \
	-Wl,-rpath,'$$ORIGIN' \
	-o $(APP)

run: all
	./$(APP) "HelloWorld"

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all run clean