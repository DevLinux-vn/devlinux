CC = gcc

CFLAG = -Wall -Wextra -Werror -std=gnu99 -Iinclude
CFLAGS_PIC = $(CFLAGS) -fPIC

.PHONY: all clean run static shared rebuild

INCLUDE_DIR = include
LIB_DIR = lib
SRC_DIR = src

BIN_DIR_STATIC = bin_static
BIN_DIR_SHARED = bin_shared
BIN_MAIN = bin_main

LIB_CALC_NAME = calc
LIB_LOGGER_NAME = logger

TARGET = $(BIN_MAIN)/main


static: $(INCLUDE_DIR)/calc.h $(SRC_DIR)/calc.c
	mkdir -p $(BIN_DIR_STATIC)
	$(CC) $(CFLAG) -c $(SRC_DIR)/calc.c -o $(BIN_DIR_STATIC)/calc.o
	ar rcs $(BIN_DIR_STATIC)/lib$(LIB_CALC_NAME).a $(BIN_DIR_STATIC)/calc.o


shared: $(INCLUDE_DIR)/logger.h $(SRC_DIR)/logger.c
	mkdir -p $(BIN_DIR_SHARED)
	$(CC) $(PICFLAG) -c $(SRC_DIR)/logger.c -o $(BIN_DIR_SHARED)/logger.o
	$(CC) -shared $(BIN_DIR_SHARED)/logger.o -o $(BIN_DIR_SHARED)/lib$(LIB_LOGGER_NAME).so


$(TARGET): static shared
	mkdir -p $(BIN_MAIN)
	$(CC) $(CFLAG) -c $(SRC_DIR)/main.c -o $(BIN_MAIN)/main.o
	$(CC) $(BIN_MAIN)/main.o \
		-L$(BIN_DIR_STATIC) -l$(LIB_CALC_NAME) \
		-L$(BIN_DIR_SHARED) -l$(LIB_LOGGER_NAME) \
		-o $(TARGET)


all: static shared $(TARGET)


run: $(TARGET)
	LD_LIBRARY_PATH=$(BIN_DIR_SHARED) ./$(TARGET)


rebuild: clean
	make all


clean:
	rm -rf $(BIN_DIR_STATIC) $(BIN_DIR_SHARED) $(BIN_MAIN)