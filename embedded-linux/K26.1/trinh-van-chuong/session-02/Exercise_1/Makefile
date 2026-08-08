CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

# Đường dẫn các file thực tế
LIB_NAME = lib/libmathutils.a
EXEC = main

all: $(EXEC)

# 1. Biên dịch các file .c của thư viện thành file .o trong obj/
obj/mathutils.o: src/mathutils.c
	$(CC) $(CFLAGS) -c src/mathutils.c -o obj/mathutils.o

# 2. Đóng gói file .o thành thư viện tĩnh .a bằng lệnh ar
$(LIB_NAME): obj/mathutils.o
	ar rcs $(LIB_NAME) obj/mathutils.o

# 3. Biên dịch chương trình chính và liên kết (link) với thư viện tĩnh
$(EXEC): app/main.c $(LIB_NAME)
	$(CC) $(CFLAGS) app/main.c -Llib -lmathutils -o $(EXEC)

run: $(EXEC)
	./$(EXEC)

clean:
	rm -rf obj/*.o lib/*.a $(EXEC)

.PHONY: all clean run