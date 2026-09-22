INC_DIR := include
SRC_DIR := src
APP_DIR := app
OBJ_DIR := obj
LIB_DIR := lib

BIN_NAME := main
LIB_NAME := mathutils
STATIC_LIB := $(LIB_DIR)/lib$(LIB_NAME).a

all: create_dirs build_lib build_main

create_dirs:
	mkdir -p $(OBJ_DIR) $(LIB_DIR)

build_lib: create_dirs
	gcc -c $(SRC_DIR)/mathutils.c -o $(OBJ_DIR)/mathutils.o -I$(INC_DIR)
	ar rcs $(STATIC_LIB) $(OBJ_DIR)/mathutils.o

build_main: build_lib
	gcc $(APP_DIR)/main.c -o $(BIN_NAME) -I$(INC_DIR) -L$(LIB_DIR) -l$(LIB_NAME)

run: all
	./$(BIN_NAME)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_NAME)

