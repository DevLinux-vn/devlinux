CC = gcc
CFLAGS = -Wall -Iinclude

OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

CALC_OBJ = $(OBJ_DIR)/calc.o
LOGGER_OBJ = $(OBJ_DIR)/logger.o

STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so

TARGET = $(BIN_DIR)/main

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

$(CALC_OBJ): src/calc.c include/calc.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/calc.c -o $(CALC_OBJ)

$(LOGGER_OBJ): src/logger.c include/logger.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -fPIC -c src/logger.c -o $(LOGGER_OBJ)

$(STATIC_LIB): $(CALC_OBJ) | $(LIB_DIR)
	ar rcs $(STATIC_LIB) $(CALC_OBJ)

$(SHARED_LIB): $(LOGGER_OBJ) | $(LIB_DIR)
	$(CC) -shared -o $(SHARED_LIB) $(LOGGER_OBJ)

$(TARGET): src/main.c $(STATIC_LIB) $(SHARED_LIB) | $(BIN_DIR)
	$(CC) src/main.c -Iinclude -L$(LIB_DIR) -Wl,-Bstatic -lcalc -Wl,-Bdynamic -llogger -lm -o $(TARGET)

run: $(TARGET)
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)

rebuild: clean all