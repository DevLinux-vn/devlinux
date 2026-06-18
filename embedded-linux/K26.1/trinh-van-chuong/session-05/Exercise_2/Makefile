# Định nghĩa trình biên dịch và các cờ (flags)
CC = gcc
CFLAGS = -Wall -pthread

# Target mặc định khi gõ lệnh 'make'
all: print_queue

# Quy tắc tạo ra file thực thi từ file .c
print_queue: print_queue.c
	$(CC) $(CFLAGS) -o print_queue print_queue.c

# Quy tắc dọn dẹp các file rác khi gõ 'make clean'
clean:
	rm -f print_queue

.PHONY: all clean