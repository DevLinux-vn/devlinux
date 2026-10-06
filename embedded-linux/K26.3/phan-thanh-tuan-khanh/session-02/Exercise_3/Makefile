CC = gcc

CFLAGS = -Wall -Wextra -Iinclude
PICFLAGS = -fPIC

OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so
TARGET = $(BIN_DIR)/main

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRC))

CALC_OBJ = $(OBJ_DIR)/calc.o
LOGGER_OBJ = $(OBJ_DIR)/logger.o
MAIN_OBJ = $(OBJ_DIR)/main.o

.PHONY: all libs static shared run clean rebuild

all: libs $(TARGET)

libs: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

$(STATIC_LIB): $(CALC_OBJ)
	mkdir -p $(LIB_DIR)
	ar rcs $@ $^

$(SHARED_LIB): $(LOGGER_OBJ)
	mkdir -p $(LIB_DIR)
	$(CC) -shared -o $@ $^

$(TARGET): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(MAIN_OBJ) -L$(LIB_DIR) -lcalc -llogger -lm -Wl,-rpath,'$$ORIGIN/../lib' -o $@

$(OBJ_DIR)/%.o: src/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LOGGER_OBJ): src/logger.c include/logger.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(PICFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

rebuild: clean all