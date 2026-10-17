CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

OBJ_DIR = obj
LIB_DIR = lib

LIBRARY = $(LIB_DIR)/libmathutils.a
APP = main

all: $(APP)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(OBJ_DIR)/mathutils.o: src/mathutils.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBRARY): $(OBJ_DIR)/mathutils.o | $(LIB_DIR)
	ar rcs $@ $^

$(APP): app/main.c $(LIBRARY)
	$(CC) $(CFLAGS) app/main.c -L$(LIB_DIR) -lmathutils -o $(APP)

run: $(APP)
	./$(APP)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(APP)

.PHONY: all run clean