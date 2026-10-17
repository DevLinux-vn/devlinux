CC      = gcc
CFLAGS  = -Wall -Wextra -O2
LDLIBS  = -lrt -lpthread
TARGETS = controller device

all: $(TARGETS)

controller: controller.c
	$(CC) $(CFLAGS) -o $@ controller.c $(LDLIBS)

device: device.c
	$(CC) $(CFLAGS) -o $@ device.c $(LDLIBS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
