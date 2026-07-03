CC = gcc
# BẮT BUỘC: Thêm thư viện liên kết thời gian thực -lrt và luồng -lpthread chuẩn doanh nghiệp
CFLAGS = -Wall -Wextra -O2 -std=gnu99 -g
LIBS = -lrt -lpthread

all: controller device

controller: controller.c
	$(CC) $(CFLAGS) -o controller controller.c $(LIBS)

device: device.c
	$(CC) $(CFLAGS) -o device device.c $(LIBS)

clean:
	rm -f controller device

.PHONY: all clean