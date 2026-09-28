CC = gcc
CFLAGS = -Wall -Iinclude

OBJ_DIR = obj
LIB_DIR = lib

OBJ = $(OBJ_DIR)/mathutils.o
LIB = $(LIB_DIR)/libmathutils.a
TARGET = main

all: $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(OBJ): src/mathutils.c include/mathutils.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/mathutils.c -o $(OBJ)

$(LIB): $(OBJ) | $(LIB_DIR)
	ar rcs $(LIB) $(OBJ)

$(TARGET): app/main.c $(LIB)
	$(CC) app/main.c -Iinclude -L$(LIB_DIR) -lmathutils -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(TARGET)