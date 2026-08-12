.PHONY: all libs static shared run clean rebuild

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -Llib

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

$(OBJ) $(LIB) $(BIN):
	mkdir -p $@

$(CALC_OBJ): src/calc.c include/calc.h | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(LOGGER_OBJ): src/logger.c include/logger.h | $(OBJ)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(MAIN_OBJ): src/main.c include/calc.h include/logger.h | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC_LIB): $(CALC_OBJ) | $(LIB)
	ar rcs $@ $^

$(SHARED_LIB): $(LOGGER_OBJ) | $(LIB)
	$(CC) -shared -o $@ $^

$(TARGET): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB) | $(BIN)
	$(CC) $(MAIN_OBJ) $(STATIC_LIB) -L$(LIB) -llogger -lm \
		-Wl,-rpath,'$$ORIGIN/../lib' -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ) $(LIB) $(BIN) app.log

rebuild: clean all
