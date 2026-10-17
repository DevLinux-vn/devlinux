CC := gcc

CPPFLAGS := -D_POSIX_C_SOURCE=200809L
CFLAGS := -Wall -Wextra -Werror -std=c11

WRITER_TARGET := config-writer
READER_TARGET := config-reader

WRITER_SRC := writer.c
READER_SRC := reader.c

COMMON_HEADER := device_cfg.h

TARGETS := $(WRITER_TARGET) $(READER_TARGET)

.PHONY: all clean

all: $(TARGETS)

$(WRITER_TARGET): $(WRITER_SRC) $(COMMON_HEADER)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(WRITER_SRC)

$(READER_TARGET): $(READER_SRC) $(COMMON_HEADER)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(READER_SRC)

clean:
	rm -f $(TARGETS)
	rm -f /tmp/device.cfg