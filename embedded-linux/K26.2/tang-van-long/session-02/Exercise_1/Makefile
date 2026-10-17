CUR_DIR :=.
APP_DIR := $(CUR_DIR)/app
INC_DIR := $(CUR_DIR)/include
LIB_DIR := $(CUR_DIR)/lib
OBJ_DIR := $(CUR_DIR)/obj
SRC_DIR := $(CUR_DIR)/src

LIB_NAME := mathutils
CC:=gcc
CFLAGS:= -I $(INC_DIR)

all: main

make_dirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(LIB_DIR)

mathutils.o: $(SRC_DIR)/mathutils.c make_dirs
	$(CC) -c $(SRC_DIR)/mathutils.c -o $(OBJ_DIR)/mathutils.o $(CFLAGS)

libmathutils.a: mathutils.o
	ar rcs $(LIB_DIR)/lib$(LIB_NAME).a $(OBJ_DIR)/mathutils.o

main: $(APP_DIR)/main.c libmathutils.a
	$(CC) $(APP_DIR)/main.c $(CFLAGS) -L$(LIB_DIR) -l$(LIB_NAME) -o main

run:
	$(CUR_DIR)/main

clean:
	rm -rf obj lib main