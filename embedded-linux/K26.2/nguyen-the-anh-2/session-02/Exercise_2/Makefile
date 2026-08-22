CC = gcc
CFLAGS = -I include
OBJ_DIR = build
BIN_DIR = bin
SRC_DIR = src
APP_DIR = app
LIB_NAME = stringutils

create_obj:
	@mkdir -p $(OBJ_DIR)
	$(CC) -c -fPIC $(SRC_DIR)/stringutils.c -o $(OBJ_DIR)/stringutils.o $(CFLAGS)


create_lib:
	@mkdir -p $(BIN_DIR)
	$(CC) -shared $(OBJ_DIR)/stringutils.o -o $(BIN_DIR)/lib$(LIB_NAME).so

build_main:
	@mkdir -p $(BIN_DIR)
	$(CC) $(APP_DIR)/main.c $(CFLAGS) -L$(BIN_DIR) -l$(LIB_NAME) -o $(BIN_DIR)/main

verify:
	LD_LIBRARY_PATH=./$(BIN_DIR) ldd $(BIN_DIR)/main

all: create_obj create_lib build_main verify


run:
	LD_LIBRARY_PATH=./$(BIN_DIR) ./$(BIN_DIR)/main "Hello_Linux"

clean:
	rm -rf ./$(OBJ_DIR)
	rm -rf ./$(BIN_DIR)