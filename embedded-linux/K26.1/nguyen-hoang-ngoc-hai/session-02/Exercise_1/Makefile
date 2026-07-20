CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -std=gnu99 -Werror

OBJ_DIR = obj
LIB_DIR = lib

LIB = $(LIB_DIR)/libmathutils.a
OBJ = $(OBJ_DIR)/mathutils.o
APP = main

all: $(APP)

$(APP): app/main.c $(LIB)
	$(CC) app/main.c $(CFLAGS) -L$(LIB_DIR) -lmathutils -o $(APP)

$(LIB): $(OBJ)
	mkdir -p $(LIB_DIR)
	ar rcs $(LIB) $(OBJ)

$(OBJ): src/mathutils.c include/mathutils.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/mathutils.c -o $(OBJ)

run: all
	./$(APP)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(APP)

.PHONY: all run clean