CC = gcc
# SỬA LỖI: Thêm -Werror ép toàn bộ cảnh báo phải xử lý như lỗi hệ thống
CFLAGS = -Wall -Wextra -Werror -O2 -std=gnu99 -g

all: sensor-daemon sensor-cli

sensor-daemon: daemon.c
	$(CC) $(CFLAGS) -o sensor-daemon daemon.c

sensor-cli: cli.c
	$(CC) $(CFLAGS) -o sensor-cli cli.c

clean:
	rm -f sensor-daemon sensor-cli

.PHONY: all clean