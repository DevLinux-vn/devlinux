CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Werror -std=gnu99
PICFLAGS = -fPIC

OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so
APP = $(BIN_DIR)/main

CALC_OBJ = $(OBJ_DIR)/calc.o
LOGGER_OBJ = $(OBJ_DIR)/logger.o
MAIN_OBJ = $(OBJ_DIR)/main.o

all: libs $(APP)

libs: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(CALC_OBJ): src/calc.c include/calc.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/calc.c -o $(CALC_OBJ)

$(LOGGER_OBJ): src/logger.c include/logger.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(PICFLAGS) -c src/logger.c -o $(LOGGER_OBJ)

$(MAIN_OBJ): src/main.c include/calc.h include/logger.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/main.c -o $(MAIN_OBJ)

$(STATIC_LIB): $(CALC_OBJ) | $(LIB_DIR)
	ar rcs $(STATIC_LIB) $(CALC_OBJ)

$(SHARED_LIB): $(LOGGER_OBJ) | $(LIB_DIR)
	$(CC) -shared -o $(SHARED_LIB) $(LOGGER_OBJ)

$(APP): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB) | $(BIN_DIR)
	$(CC) $(MAIN_OBJ) -L$(LIB_DIR) -lcalc -llogger -Wl,-rpath,'$$ORIGIN/../lib' -lm -o $(APP)

run: all
	./$(APP)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

rebuild: clean all

.PHONY: all libs static shared run clean rebuild