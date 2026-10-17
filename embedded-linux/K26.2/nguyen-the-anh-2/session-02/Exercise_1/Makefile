CC = gcc
CFLAGS = -I include
OBJ_DIR = obj
SRC_DIR = src
APP_DIR = app
LIB_DIR = lib
LIB_NAME = mathutils

# --- CÁC TARGET HÀNH ĐỘNG ---

create_obj:
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(SRC_DIR)/mathutils.c -o $(OBJ_DIR)/mathutils.o $(CFLAGS)

create_lib:
	@mkdir -p $(LIB_DIR)
	ar rcs $(LIB_DIR)/lib$(LIB_NAME).a $(OBJ_DIR)/mathutils.o

build_main:
	$(CC) $(APP_DIR)/main.c -L$(LIB_DIR) -l$(LIB_NAME) $(CFLAGS) -o main

all: create_obj create_lib build_main

run:
	./main

clean:
	rm -rf ./$(OBJ_DIR)
	rm -rf ./$(LIB_DIR)
	rm -f main