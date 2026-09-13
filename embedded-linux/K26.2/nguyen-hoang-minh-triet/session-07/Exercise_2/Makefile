# Trình biên dịch và cờ biên dịch
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

# Tên file thực thi
TARGET = iot_gateway

# Danh sách source files
SRC = main.c

.PHONY: all clean

# Target mặc định
all: $(TARGET)

# Quy tắc build file thực thi
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Target dọn dẹp
clean:
	rm -f $(TARGET)

	