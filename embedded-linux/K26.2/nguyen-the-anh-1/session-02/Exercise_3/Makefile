CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

OBJ = obj
LIB = lib
BIN = bin

CALC_OBJ = $(OBJ)/calc.o
LOGGER_OBJ = $(OBJ)/logger.o
MAIN_OBJ = $(OBJ)/main.o

STATIC_LIB = $(LIB)/libcalc.a
SHARED_LIB = $(LIB)/liblogger.so
TARGET = $(BIN)/main

all: libs $(TARGET)

libs: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

$(OBJ)/%.o: src/%.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(STATIC_LIB): $(CALC_OBJ)
	ar rcs $@ $^

$(SHARED_LIB): $(LOGGER_OBJ)
	$(CC) -shared -o $@ $^

$(MAIN_OBJ): src/main.c include/calc.h include/logger.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB)
	$(CC) $(MAIN_OBJ) -L$(LIB) -Wl,-Bstatic -lcalc -Wl,-Bdynamic -llogger -Wl,-rpath,'$$ORIGIN/../lib' -lm -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ)/* $(LIB)/* $(BIN)/* app.log

rebuild: clean all

.PHONY: all libs static shared run clean rebuild
