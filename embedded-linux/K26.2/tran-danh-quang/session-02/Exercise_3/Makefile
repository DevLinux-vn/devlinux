CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -Llib -lcalc -llogger -lm

all: bin/main

libs: static shared

static: lib/libcalc.a

shared: lib/liblogger.so

lib/libcalc.a: obj/calc.o | lib
	ar rcs $@ $<

lib/liblogger.so: obj/logger.o | lib
	$(CC) -shared -o $@ $<

obj/calc.o: src/calc.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/logger.o: src/logger.c | obj
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

bin/main: src/main.c libs | bin
	$(CC) src/main.c $(CFLAGS) $(LDFLAGS) -o $@ -Wl,-rpath,'$$ORIGIN/../lib'

obj lib bin:
	mkdir -p $@

run: all
	LD_LIBRARY_PATH=$$(pwd)/lib bin/main

clean:
	rm -rf obj lib bin app.log

rebuild: clean all

.PHONY: all libs static shared run clean rebuild
