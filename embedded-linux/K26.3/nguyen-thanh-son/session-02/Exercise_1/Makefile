.PHONY: all prerequisites create_obj create_lib run clean

CUR_DIR := .
APP_DIR := $(CUR_DIR)/app
BIN_DIR := $(CUR_DIR)/bin
INC_DIR := $(CUR_DIR)/include
LIB_DIR := $(CUR_DIR)/lib
OBJ_DIR := $(CUR_DIR)/obj
SRC_DIR := $(CUR_DIR)/src

LIB_NAME := mathutils
BIN_NAME := use_mathutils

all: prerequisites create_obj create_lib
	gcc $(APP_DIR)/main.c -I$(INC_DIR) -L$(LIB_DIR) -l$(LIB_NAME) -o $(BIN_DIR)/$(BIN_NAME)

prerequisites:
	mkdir -p $(BIN_DIR) $(OBJ_DIR) $(LIB_DIR)

create_obj:
	gcc -c $(SRC_DIR)/mathutils.c -I$(INC_DIR) -o $(OBJ_DIR)/mathutils.o

create_lib:
	ar rcs $(LIB_DIR)/lib$(LIB_NAME).a $(OBJ_DIR)/mathutils.o
	
run: all
	$(BIN_DIR)/$(BIN_NAME) 

clean: 
	rm -f $(OBJ_DIR)/*.o
	rm -f $(LIB_DIR)/*
	rm -f $(BIN_DIR)/*
