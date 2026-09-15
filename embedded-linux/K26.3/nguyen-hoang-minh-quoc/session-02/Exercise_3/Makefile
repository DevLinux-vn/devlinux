CC := gcc
CFLAGS := -Wall -Wextra -Iinclude
PICFLAGS := -fPIC
OBJ_DIR := obj
LIB_DIR := lib
BIN_DIR := bin

STATIC_LIB := $(LIB_DIR)/libcalc.a
SHARED_LIB := $(LIB_DIR)/liblogger.so
TARGET := $(BIN_DIR)/main

CALC_OBJ := $(OBJ_DIR)/calc.o
LOGGER_OBJ := $(OBJ_DIR)/logger.o

all: $(TARGET)

libs: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

$(TARGET): src/main.c $(STATIC_LIB) $(SHARED_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) src/main.c -L$(LIB_DIR) -Wl,-rpath,'$$ORIGIN/../lib' -lcalc -llogger -lm -o $@

$(STATIC_LIB): $(CALC_OBJ) | $(LIB_DIR)
	ar rcs $@ $<

$(SHARED_LIB): $(LOGGER_OBJ) | $(LIB_DIR)
	$(CC) -shared -o $@ $<

$(CALC_OBJ): src/calc.c include/calc.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LOGGER_OBJ): src/logger.c include/logger.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(PICFLAGS) -c $< -o $@

$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	mkdir -p $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

rebuild: clean all

.PHONY: all libs static shared run clean rebuild
