CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

BUILD_DIR = build
BIN_DIR = bin

SHARED_LIB = $(BIN_DIR)/libstringutils.so
EXEC = $(BIN_DIR)/main

# Chuỗi mẫu mặc định để test khi gõ 'make run'
SAMPLE_STR = "Hello DevLinux 2026"

all: $(EXEC)

$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Biên dịch với cờ -fPIC (Position Independent Code) bắt buộc cho shared lib
$(BUILD_DIR)/stringutils.o: src/stringutils.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fPIC -c src/stringutils.c -o $@

# Tạo thư viện động .so từ object file
$(SHARED_LIB): $(BUILD_DIR)/stringutils.o | $(BIN_DIR)
	$(CC) -shared -o $@ $<

# Biên dịch main và liên kết động với libstringutils
$(EXEC): main.c $(SHARED_LIB)
	$(CC) $(CFLAGS) main.c -L$(BIN_DIR) -lstringutils -o $@

# Chạy kèm LD_LIBRARY_PATH để hệ thống định vị được file .so trong bin/
run: $(EXEC)
	LD_LIBRARY_PATH=$(BIN_DIR) ./$(EXEC) $(SAMPLE_STR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all run clean