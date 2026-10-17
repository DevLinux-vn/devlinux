CUR_DIR := .
INCLUDE_DIR := $(CUR_DIR)/include
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
LIB_DIR := $(CUR_DIR)/lib
BIN_DIR := $(CUR_DIR)/bin

CC = gcc
CFLAGS = -I $(INCLUDE_DIR) -fPIC

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: libs app verify

libs: static shared

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

static: $(OBJ_DIR)/calc.o
	@mkdir -p $(LIB_DIR)
	ar rcs $(LIB_DIR)/libcalc.a $(OBJ_DIR)/calc.o

shared: $(OBJ_DIR)/logger.o
	@mkdir -p $(LIB_DIR)
	$(CC) -shared $(OBJ_DIR)/logger.o -o $(LIB_DIR)/liblogger.so

app: $(OBJS) static shared
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ_DIR)/main.o -o $(BIN_DIR)/app -L$(LIB_DIR) -lcalc -llogger -Wl,-rpath=$(LIB_DIR)
verify:
	@echo " Kiem tra file thuc thi (ldd) "
	ldd $(BIN_DIR)/app
	@echo " Kiem tra thu vien tinh (nm) "
	nm $(LIB_DIR)/libcalc.a

run: app
	./$(BIN_DIR)/app

clean:
	rm -rf $(LIB_DIR) $(OBJ_DIR) $(BIN_DIR) app.log

rebuild: clean all