CC=gcc
CFLAGS=-Wall -Wextra -Werror -g -std=gnu99
LDFLAGS=-lrt -lpthread

all: controller device

controller: controller.c
	$(CC) $(CFLAGS) controller.c -o controller $(LDFLAGS)

device: device.c
	$(CC) $(CFLAGS) device.c -o device $(LDFLAGS)

clean:
	rm -f controller device