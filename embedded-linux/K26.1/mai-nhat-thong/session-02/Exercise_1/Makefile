CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -std=gnu99 -O2 -Iinclude
AR = ar
ARFLAGS = rcs

SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
APP_DIR = app

SRC = $(SRC_DIR)/mathutils.c
OBJ = $(OBJ_DIR)/mathutils.o
LIB = $(LIB_DIR)/libmathutils.a
TARGET = main

.PHONY: all clean run

all: $(TARGET)

$(OBJ): $(SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ)
	@mkdir -p $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $<

$(TARGET): $(APP_DIR)/main.c $(LIB)
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lmathutils -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(TARGET)