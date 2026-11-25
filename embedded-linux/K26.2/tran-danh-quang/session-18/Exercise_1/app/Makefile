.DEFAULT_GOAL := all
# Preserve a Yocto SDK's exported CC, including its sysroot arguments.
ifeq ($(origin CC),default)
CC = $(CROSS_COMPILE)gcc
endif
CPPFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic
TARGET := ssd1306_app
.PHONY: all clean
all: $(TARGET)

$(TARGET): ssd1306_app.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ssd1306_app.o: ssd1306_app.c ../driver/ssd1306.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c -o $@ $<

clean:
	$(RM) $(TARGET) ssd1306_app.o ssd1306_app.d

-include ssd1306_app.d
