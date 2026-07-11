CC := gcc

CPPFLAGS := -D_POSIX_C_SOURCE=200809L
CFLAGS := -Wall -Wextra -Werror -std=c11
LDLIBS := -lrt -lpthread

CONTROLLER_TARGET := controller
DEVICE_TARGET := device

CONTROLLER_SRC := controller.c
DEVICE_SRC := device.c

TARGETS := $(CONTROLLER_TARGET) $(DEVICE_TARGET)

.PHONY: all clean

all: $(TARGETS)

$(CONTROLLER_TARGET): $(CONTROLLER_SRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDLIBS)

$(DEVICE_TARGET): $(DEVICE_SRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDLIBS)

clean:
	rm -f $(TARGETS)
	rm -f /dev/shm/device_shm