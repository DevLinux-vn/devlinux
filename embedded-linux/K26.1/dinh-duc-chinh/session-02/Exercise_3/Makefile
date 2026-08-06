CC       = gcc
CFLAGS   = -Wall -I include
LDFLAGS  = -L lib

SRC_DIR  = src
OBJ_DIR  = obj
LIB_DIR  = lib
BIN_DIR  = bin

# Phân loại source
CALC_SRC   = $(SRC_DIR)/calc.c
LOGGER_SRC = $(SRC_DIR)/logger.c
MAIN_SRC   = $(SRC_DIR)/main.c

# Mapping .c -> .o (pattern)
CALC_OBJ   = $(OBJ_DIR)/calc.o
LOGGER_OBJ = $(OBJ_DIR)/logger.o

STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so
EXE        = $(BIN_DIR)/app

all: $(EXE)

libs: static shared
static: $(STATIC_LIB)
shared: $(SHARED_LIB)

# Pattern rule cho object thường
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Logger cần -fPIC vì sẽ vào .so → override riêng
$(LOGGER_OBJ): $(LOGGER_SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(STATIC_LIB): $(CALC_OBJ) | $(LIB_DIR)
	ar rcs $@ $^

$(SHARED_LIB): $(LOGGER_OBJ) | $(LIB_DIR)
	$(CC) -shared $< -o $@

$(EXE): $(MAIN_SRC) $(STATIC_LIB) $(SHARED_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -lcalc -llogger \
		-Wl,-rpath,'$$ORIGIN/../lib' -o $@

$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	mkdir -p $@

run: $(EXE)
	./$(EXE)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

rebuild: clean all

.PHONY: all libs static shared run clean rebuild
