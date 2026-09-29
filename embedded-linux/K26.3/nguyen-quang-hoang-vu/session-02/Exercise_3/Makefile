CC = gcc
CFLAGS = -Iinclude -Wall

all: bin/main

libs: static shared

static: lib/libcalc.a

shared: lib/liblogger.so

obj/calc.o: src/calc.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c src/calc.c -o obj/calc.o

lib/libcalc.a: obj/calc.o
	mkdir -p lib
	ar rcs lib/libcalc.a obj/calc.o

lib/liblogger.so: src/logger.c
	mkdir -p lib
	$(CC) $(CFLAGS) -fPIC -shared -o lib/liblogger.so src/logger.c

bin/main: lib/libcalc.a lib/liblogger.so src/main.c
	mkdir -p bin
	$(CC) $(CFLAGS) src/main.c -Llib -lcalc -llogger -lm -o bin/main

run: bin/main
	LD_LIBRARY_PATH=lib ./bin/main

clean:
	rm -rf obj lib bin app.log

rebuild: clean all

.PHONY: all libs static shared run clean rebuild
