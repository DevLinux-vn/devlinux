# Khai báo trình biên dịch và cờ biên dịch (bật warning all và warning extra)
CC = gcc
CFLAGS = -Wall -Wextra

# Target mặc định khi gõ lệnh 'make' hoặc 'make all'
all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c -lm

# Target dọn dẹp file thực thi sau khi build
clean:
	rm -f main