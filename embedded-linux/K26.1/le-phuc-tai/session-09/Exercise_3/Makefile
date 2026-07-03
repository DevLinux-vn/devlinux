CC = gcc
# FIX: Added strict -Werror to ensure zero-warning enterprise compilation
CFLAGS = -Wall -Wextra -Werror -O2 -std=gnu99 -g
LIBS = -lrt -lpthread

all: controller device

controller: controller.c
	$(CC) $(CFLAGS) -o controller controller.c $(LIBS)

device: device.c
	$(CC) $(CFLAGS) -o device device.c $(LIBS)

clean:
	rm -f controller device

.PHONY: all clean