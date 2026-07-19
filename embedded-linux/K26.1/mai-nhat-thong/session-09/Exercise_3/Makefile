CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
LIBS = -lpthread -lrt
TARGETS = controller device

all: $(TARGETS)

controller: controller.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

device: device.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean