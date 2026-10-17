CC      = gcc
CFLAGS  = -Wall -Wextra -O2
TARGETS = config-writer config-reader

all: $(TARGETS)

config-writer: writer.c device_cfg.h
	$(CC) $(CFLAGS) -o $@ writer.c

config-reader: reader.c device_cfg.h
	$(CC) $(CFLAGS) -o $@ reader.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
