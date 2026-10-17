CC := gcc
CFLAGS := -Wall -Wextra -Iinclude
TARGET := main
OBJ_DIR := obj
LIB_DIR := lib
LIB := $(LIB_DIR)/libmathutils.a

APP_SRC := app/main.c
MATH_OBJ := $(OBJ_DIR)/mathutils.o

all: $(TARGET)

$(TARGET): $(APP_SRC) $(LIB)
	$(CC) $(CFLAGS) $(APP_SRC) -L$(LIB_DIR) -lmathutils -o $@

$(LIB): $(MATH_OBJ) | $(LIB_DIR)
	ar rcs $@ $<

$(MATH_OBJ): src/mathutils.c include/mathutils.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

run: all
	printf "5 3\n5\n" | ./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(TARGET)

.PHONY: all run clean
