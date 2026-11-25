.DEFAULT_GOAL := all
CC = $(CROSS_COMPILE)gcc
CPPFLAGS += -D_GNU_SOURCE
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic
TARGET := led_app

.PHONY: all clean
all: $(TARGET)

$(TARGET): main.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

main.o: main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c -o $@ $<

clean:
	$(RM) $(TARGET) main.o main.d

-include main.d
