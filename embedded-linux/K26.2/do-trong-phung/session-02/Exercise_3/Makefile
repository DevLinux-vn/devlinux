CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

CALC_OBJ = $(OBJ_DIR)/calc.o
LOGGER_OBJ = $(OBJ_DIR)/logger.o
MAIN_OBJ = $(OBJ_DIR)/main.o

STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so
TARGET = $(BIN_DIR)/main

.PHONY: all libs static shared run clean rebuild

all: $(TARGET)

libs: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

$(STATIC_LIB): $(CALC_OBJ)
	mkdir -p $(LIB_DIR)
	ar rcs $@ $^

$(SHARED_LIB): $(LOGGER_OBJ)
	mkdir -p $(LIB_DIR)
	$(CC) -shared -o $@ $^

$(CALC_OBJ): src/calc.c include/calc.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LOGGER_OBJ): src/logger.c include/logger.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(MAIN_OBJ): src/main.c include/calc.h include/logger.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB)
	mkdir -p $(BIN_DIR)
	$(CC) $(MAIN_OBJ) -L$(LIB_DIR) -lcalc -llogger -o $@

run: $(TARGET)
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)

rebuild: clean all