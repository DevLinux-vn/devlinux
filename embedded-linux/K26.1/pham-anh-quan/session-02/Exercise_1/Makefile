# Compiler và Flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

# Thư mục đầu ra
OBJ_DIR = obj
LIB_DIR = lib

# Targets chính
STATIC_LIB = $(LIB_DIR)/libmathutils.a
EXEC = main

# Target mặc định khi gõ 'make'
all: $(EXEC)

# 1. Tạo thư mục tạm nếu chưa có
$(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

# 2. Biên dịch mathutils.c thành object file (.o)
$(OBJ_DIR)/mathutils.o: src/mathutils.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/mathutils.c -o $@

# 3. Đóng gói thành static library (.a)
$(STATIC_LIB): $(OBJ_DIR)/mathutils.o | $(LIB_DIR)
	ar rcs $@ $<

# 4. Biên dịch chương trình chính và liên kết với static library
$(EXEC): app/main.c $(STATIC_LIB)
	$(CC) $(CFLAGS) app/main.c -L$(LIB_DIR) -lmathutils -o $@

# Target 'make run' để chạy chương trình
run: $(EXEC)
	./$(EXEC)

# Target 'make clean' để dọn dẹp hệ thống
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(EXEC)

.PHONY: all run clean