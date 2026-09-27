CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
INC_DIR := $(CUR_DIR)/include
LIB_DIR := $(CUR_DIR)/lib
OBJ_DIR := $(CUR_DIR)/obj
SRC_DIR := $(CUR_DIR)/src

CC      := gcc
AR      := ar
ARFLAGS := rcs
CFLAGS  := -Wall -Wextra -std=c11 -I$(INC_DIR) -MMD -MP

STATIC_NAME := calc
SHARED_NAME := logger
BIN_NAME    := use_calc_logger
LOG_FILE    := app.log

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

STATIC_OBJ := $(OBJ_DIR)/$(STATIC_NAME).o
SHARED_OBJ := $(OBJ_DIR)/$(SHARED_NAME).o
MAIN_OBJ   := $(OBJ_DIR)/main.o

STATIC_LIB := $(LIB_DIR)/lib$(STATIC_NAME).a
SHARED_LIB := $(LIB_DIR)/lib$(SHARED_NAME).so
BIN        := $(BIN_DIR)/$(BIN_NAME)

.PHONY: all libs static shared run clean rebuild

all: $(BIN)

libs: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

# -Bstatic/-Bdynamic force libcalc to be baked in and liblogger to stay dynamic.
# rpath lets bin/ find lib/liblogger.so at runtime without LD_LIBRARY_PATH.
$(BIN): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB) | $(BIN_DIR)
	$(CC) $(MAIN_OBJ) -L$(LIB_DIR) \
		-Wl,-Bstatic -l$(STATIC_NAME) -Wl,-Bdynamic -l$(SHARED_NAME) \
		-Wl,-rpath,'$$ORIGIN/../lib' -o $@

$(STATIC_LIB): $(STATIC_OBJ) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^

$(SHARED_LIB): $(SHARED_OBJ) | $(LIB_DIR)
	$(CC) -shared -o $@ $^

# Only the shared library object needs position-independent code
$(SHARED_OBJ): CFLAGS += -fPIC

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(LIB_DIR) $(OBJ_DIR):
	mkdir -p $@

run: all
	$(BIN)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) $(LOG_FILE)

rebuild:
	$(MAKE) clean
	$(MAKE) all

-include $(DEPS)
