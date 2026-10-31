CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so
EXEC = $(BIN_DIR)/main

# Mặc định gõ 'make' sẽ build tất cả thư viện và chương trình chính
all: static shared $(EXEC)

# Tạo các thư mục cần thiết
$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	mkdir -p $@

# --- BIÊN DỊCH STATIC LIBRARY ---
static: $(STATIC_LIB)

$(OBJ_DIR)/calc.o: src/calc.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/calc.c -o $@

$(STATIC_LIB): $(OBJ_DIR)/calc.o | $(LIB_DIR)
	ar rcs $@ $<

# --- BIÊN DỊCH SHARED LIBRARY (Cần -fPIC) ---
shared: $(SHARED_LIB)

$(OBJ_DIR)/logger.o: src/logger.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -fPIC -c src/logger.c -o $@

$(SHARED_LIB): $(OBJ_DIR)/logger.o | $(LIB_DIR)
	$(CC) -shared -o $@ $<

# --- LINK ĐỒNG THỜI CẢ 2 THƯ VIỆN ---
$(EXEC): src/main.c $(STATIC_LIB) $(SHARED_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) src/main.c -L$(LIB_DIR) -lcalc -llogger -o $@

# Lệnh chạy kèm thư viện động
run:
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(EXEC)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

rebuild: clean all

.PHONY: all static shared run clean rebuild