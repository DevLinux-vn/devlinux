CC      := gcc

CFLAGS  := -Wall -Wextra -Werror -std=c11 -Iinclude
PICFLAG := -fPIC

AR      := ar
ARFLAGS := rcs

OBJ_DIR := obj
LIB_DIR := lib
BIN_DIR := bin

STATIC_LIB := $(LIB_DIR)/libcalc.a
SHARED_LIB := $(LIB_DIR)/liblogger.so

TARGET := $(BIN_DIR)/main

SRC_FILES := $(wildcard src/*.c)

CALC_SRC   := src/calc.c
LOGGER_SRC := src/logger.c
MAIN_SRC   := src/main.c

CALC_OBJ   := $(OBJ_DIR)/calc.o
LOGGER_OBJ := $(OBJ_DIR)/logger.o
MAIN_OBJ   := $(OBJ_DIR)/main.o

all: libs $(TARGET)

libs: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/logger.o: src/logger.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(PICFLAG) -c $< -o $@

$(STATIC_LIB): $(CALC_OBJ) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^

$(SHARED_LIB): $(LOGGER_OBJ) | $(LIB_DIR)
	$(CC) -shared -o $@ $^

$(TARGET): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB) | $(BIN_DIR)
	$(CC) \
	$(MAIN_OBJ) \
	-L$(LIB_DIR) \
	-lcalc \
	-llogger \
	-Wl,-rpath,'$$ORIGIN/../lib' \
	-o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(LIB_DIR)
	rm -rf $(BIN_DIR)
	rm -f app.log

rebuild: clean all

.PHONY: all libs static shared run clean rebuild

