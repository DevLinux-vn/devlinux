CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
PICFLAGS = -Wall -Wextra -Iinclude -fPIC

OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so
TARGET = $(BIN_DIR)/main

all: $(TARGET)

libs: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

$(STATIC_LIB): $(OBJ_DIR)/calc.o
	mkdir -p $(LIB_DIR)
	ar rcs $(STATIC_LIB) $(OBJ_DIR)/calc.o

$(SHARED_LIB): $(OBJ_DIR)/logger.o
	mkdir -p $(LIB_DIR)
	$(CC) -shared -o $(SHARED_LIB) $(OBJ_DIR)/logger.o

$(OBJ_DIR)/calc.o: src/calc.c include/calc.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/calc.c -o $(OBJ_DIR)/calc.o

$(OBJ_DIR)/logger.o: src/logger.c include/logger.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(PICFLAGS) -c src/logger.c -o $(OBJ_DIR)/logger.o

$(OBJ_DIR)/main.o: src/main.c include/calc.h include/logger.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/main.c -o $(OBJ_DIR)/main.o

$(TARGET): $(OBJ_DIR)/main.o $(STATIC_LIB) $(SHARED_LIB)
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJ_DIR)/main.o -L$(LIB_DIR) -lcalc -llogger -Wl,-rpath,$(LIB_DIR) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

rebuild: clean all

.PHONY: all libs static shared run clean rebuild