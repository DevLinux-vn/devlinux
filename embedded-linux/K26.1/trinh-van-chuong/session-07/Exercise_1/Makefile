# Tên file thực thi đầu ra
TARGET = signal_block

# Trình biên dịch và các cờ tối ưu, cảnh báo lỗi
CC = gcc
CFLAGS = -Wall -Wextra

# Target mặc định khi gõ "make" hoặc "make all"
all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

# Target dọn dẹp các file rác/file biên dịch cũ
clean:
	rm -f $(TARGET)

.PHONY: all clean