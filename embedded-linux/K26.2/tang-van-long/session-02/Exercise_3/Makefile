CUR_DIR := .
INC_DIR := $(CUR_DIR)/include
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
LIB_DIR := $(CUR_DIR)/lib
BIN_DIR := $(CUR_DIR)/bin


STATIC_NAME := calc
SHARED_NAME := logger

CC := gcc
CFLAGS := -I $(INC_DIR)


all: main


make_dirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(LIB_DIR)
	mkdir -p $(BIN_DIR)


static: $(SRC_DIR)/calc.c make_dirs
	$(CC) -c $(SRC_DIR)/calc.c -o $(OBJ_DIR)/calc.o $(CFLAGS)
	ar rcs $(LIB_DIR)/lib$(STATIC_NAME).a $(OBJ_DIR)/calc.o


shared: $(SRC_DIR)/logger.c make_dirs
	$(CC) -fPIC -c $(SRC_DIR)/logger.c -o $(OBJ_DIR)/logger.o $(CFLAGS)
	$(CC) -shared -o $(LIB_DIR)/lib$(SHARED_NAME).so $(OBJ_DIR)/logger.o


main: static shared
	$(CC) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o $(CFLAGS)
	$(CC) $(OBJ_DIR)/main.o $(LIB_DIR)/lib$(STATIC_NAME).a -L$(LIB_DIR) -l$(SHARED_NAME) -Wl,-rpath,'./lib' -o $(BIN_DIR)/main

run:
	LD_LIBRARY_PATH=$(LIB_DIR) $(BIN_DIR)/main

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

rebuild: clean all