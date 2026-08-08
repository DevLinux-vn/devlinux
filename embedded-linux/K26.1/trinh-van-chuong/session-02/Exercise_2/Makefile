CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

LIB_NAME = bin/libstringutils.so
EXEC = bin/main_app

all: $(LIB_NAME) $(EXEC)

# 1. Biên dịch file .c của thư viện với cờ -fPIC (Position Independent Code)
build/stringutils.o: src/stringutils.c
	$(CC) $(CFLAGS) -fPIC -c src/stringutils.c -o build/stringutils.o

# 2. Tạo ra file thư viện động .so bằng cờ -shared
$(LIB_NAME): build/stringutils.o
	$(CC) -shared build/stringutils.o -o $(LIB_NAME)

# 3. Biên dịch file main và liên kết động với thư viện .so bằng cờ -L và -l
$(EXEC): src/main.c $(LIB_NAME)
	$(CC) $(CFLAGS) src/main.c -Lbin -lstringutils -o $(EXEC)

# 4. Target chạy chương trình mẫu (Phải nạp LD_LIBRARY_PATH để Linux tìm thấy file .so)
run: $(EXEC)
	LD_LIBRARY_PATH=./bin ./$(EXEC) "Hello Nghe An 2026"

clean:
	rm -rf build/*.o bin/*

.PHONY: all clean run