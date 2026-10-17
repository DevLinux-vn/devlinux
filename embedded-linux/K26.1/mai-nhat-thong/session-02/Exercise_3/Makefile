CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -std=gnu99 -O2 -Iinclude
AR = ar
ARFLAGS = rcs

OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so
TARGET = $(BIN_DIR)/app_runner

# Advanced Scalability: Dynamic source detection using wildcard
SRCS = $(wildcard src/*.c)

.PHONY: all libs static shared run clean rebuild

all: $(TARGET)

libs: static shared

static: $(OBJ_DIR)/calc.o
	@mkdir -p $(LIB_DIR)
	$(AR) $(ARFLAGS) $(STATIC_LIB) $<

$(OBJ_DIR)/calc.o: src/calc.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

shared: $(OBJ_DIR)/logger.o
	@mkdir -p $(LIB_DIR)
	$(CC) -shared -o $(SHARED_LIB) $<

$(OBJ_DIR)/logger.o: src/logger.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(TARGET): src/main.c static shared
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lcalc -llogger -Wl,-rpath,'$$ORIGIN/../lib' -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

rebuild: clean all