CC      := gcc
CFLAGS  := -Wall -Wextra -Iinclude
PICFLAG := -fPIC

OBJ_DIR := obj
LIB_DIR := lib
BIN_DIR := bin
SRC_DIR := src

TARGET := $(BIN_DIR)/app

CALC_OBJ   := $(OBJ_DIR)/calc.o
LOGGER_OBJ := $(OBJ_DIR)/logger.o
MAIN_OBJ   := $(OBJ_DIR)/main.o

STATIC_LIB := $(LIB_DIR)/libcalc.a
SHARED_LIB := $(LIB_DIR)/liblogger.so

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

$(OBJ_DIR)/calc.o: $(SRC_DIR)/calc.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/logger.o: $(SRC_DIR)/logger.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(PICFLAG) -c $< -o $@

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC_LIB): $(CALC_OBJ) | $(LIB_DIR)
	ar rcs $@ $^

$(SHARED_LIB): $(LOGGER_OBJ) | $(LIB_DIR)
	$(CC) -shared -o $@ $^

$(TARGET): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB) | $(BIN_DIR)
	$(CC) $(MAIN_OBJ) \
	-L$(LIB_DIR) \
	-Wl,-Bstatic -lcalc \
	-Wl,-Bdynamic -llogger \
	-Wl,-rpath,'$$ORIGIN/../lib' \
	-lm \
	-o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

rebuild: clean all

.PHONY: all libs static shared run clean rebuild