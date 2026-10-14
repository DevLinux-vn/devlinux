CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=gnu99 -Iinclude
.PHONY: clean run
OBJ_DIR = obj
LIB_DIR = lib


OBJ = $(OBJ_DIR)/mathutils.o
LIB = $(LIB_DIR)/libmathutils.a

TARGET = main
$(TARGET): app/main.c $(LIB)
	$(CC) $(CFLAGS) app/main.c -L$(LIB_DIR) -lmathutils -o $(TARGET)

$(LIB): $(OBJ)
	mkdir -p $(LIB_DIR)
	ar rcs $(LIB) $(OBJ)

$(OBJ): src/mathutils.c include/mathutils.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/mathutils.c -o $(OBJ)
all: $(TARGET)
run: $(TARGET)
	./$(TARGET)
	
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) main
