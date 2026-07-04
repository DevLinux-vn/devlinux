CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lrt -lpthread

TARGETS = controller device

all: $(TARGETS)

controller: controller.c
	$(CC) $(CFLAGS) -o controller controller.c $(LDFLAGS)

device: device.c
	$(CC) $(CFLAGS) -o device device.c $(LDFLAGS)

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
