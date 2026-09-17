CC = gcc
CFLAGS = -Wall -g -Iinclude
AR = ar
ARFLAGS = rcs

SRCS = $(wildcard src/*.c)
STATIC_LIB = lib/libcalc.a
SHARED_LIB = lib/liblogger.so
TARGET = bin/calc_app

all: $(TARGET)

libs: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

$(STATIC_LIB): obj/calc.o | lib
	$(AR) $(ARFLAGS) $@ $^

$(SHARED_LIB): obj/logger.o | lib
	$(CC) -shared -o $@ $^

# Pattern rules biên dịch mã nguồn thành object file
obj/calc.o: src/calc.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/logger.o: src/logger.c | obj
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

obj/main.o: src/main.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): obj/main.o $(STATIC_LIB) $(SHARED_LIB) | bin
	$(CC) $(CFLAGS) obj/main.o -Llib -lcalc -llogger -lm -Wl,-rpath,'$$ORIGIN/../lib' -o $@

obj:
	mkdir -p obj

lib:
	mkdir -p lib

bin:
	mkdir -p bin

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf obj lib bin app.log

rebuild: clean all

.PHONY: all libs static shared run clean rebuild