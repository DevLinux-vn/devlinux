# Compiler Configuration
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -pedantic -std=c99 -O2 -D_POSIX_C_SOURCE=200809L

# Libraries required for POSIX Shared Memory and Pthreads
LDLIBS  := -lrt -lpthread

# Build Targets
TARGETS := controller device

.PHONY: all clean

all: $(TARGETS)

controller: controller.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

device: device.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

clean:
	rm -f $(TARGETS) *.o