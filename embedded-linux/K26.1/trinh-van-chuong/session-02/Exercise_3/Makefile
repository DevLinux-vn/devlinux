CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

# Thư mục đầu ra
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

# File đầu ra cụ thể
STATIC_LIB = $(LIB_DIR)/libcalc.a
SHARED_LIB = $(LIB_DIR)/liblogger.so
EXEC = $(BIN_DIR)/main_app

all: $(STATIC_LIB) $(SHARED_LIB) $(EXEC)

# Target gom biên dịch cả 2 thư viện
libs: $(STATIC_LIB) $(SHARED_LIB)

# Target chỉ build riêng thư viện tĩnh
static: $(STATIC_LIB)

# Target chỉ build riêng thư viện động
shared: $(SHARED_LIB)

# Luật mẫu (Pattern Rule) biến dịch tất cả file .c trong src/ thành file .o trong obj/
# Dùng cờ -fPIC để đảm bảo file .o của thư viện động tương thích tốt
$(OBJ_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# Build thư viện tĩnh bằng lệnh ar (không hardcode)
$(STATIC_LIB): $(OBJ_DIR)/calc.o
	ar rcs $@ $<

# Build thư viện động bằng cờ -shared (không hardcode)
$(SHARED_LIB): $(OBJ_DIR)/logger.o
	$(CC) -shared -o $@ $<

# Biên dịch chương trình chính bằng cách tự động link cả 2 thư viện (.a và .so)
$(EXEC): $(OBJ_DIR)/main.o $(STATIC_LIB) $(SHARED_LIB)
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lcalc -llogger -o $@

# Chạy chương trình (Cần nạp đường dẫn thư viện động LD_LIBRARY_PATH)
run: $(EXEC)
	LD_LIBRARY_PATH=./$(LIB_DIR) ./$(EXEC)

# Xóa sạch các file rác sinh ra
clean:
	rm -rf $(OBJ_DIR)/*.o $(LIB_DIR)/* $(BIN_DIR)/* app.log

# Xóa sạch rồi build lại từ đầu
rebuild: clean all

.PHONY: all libs static shared run clean rebuild