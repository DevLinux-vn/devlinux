# Định nghĩa trình biên dịch và các cờ (flags)
CC = gcc
CFLAGS = -Wall -pthread

# Target mặc định khi gõ lệnh 'make'
all: booking

# Quy tắc tạo ra file thực thi từ file .c
booking: booking.c
	$(CC) $(CFLAGS) -o booking booking.c

# Quy tắc dọn dẹp các file rác/file thực thi khi gõ 'make clean'
clean:
	rm -f booking

.PHONY: all clean