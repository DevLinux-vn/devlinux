CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L
LDFLAGS=-lrt -lpthread

all: controller device

controller: controller.c
	$(CC) $(CFLAGS) controller.c -o controller $(LDFLAGS)

device: device.c
	$(CC) $(CFLAGS) device.c -o device $(LDFLAGS)

clean:
	rm -f controller device