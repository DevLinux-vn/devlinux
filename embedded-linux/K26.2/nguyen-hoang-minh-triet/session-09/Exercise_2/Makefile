# Compiler Configuration
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -pedantic -std=c99 -O2

# Build Targets
TARGETS := writer reader

.PHONY: all clean

all: $(TARGETS)

writer: writer.c device_cfg.h
	$(CC) $(CFLAGS) -o $@ writer.c

reader: reader.c device_cfg.h
	$(CC) $(CFLAGS) -o $@ reader.c

clean:
	rm -f $(TARGETS) *.o /tmp/device.cfg
	