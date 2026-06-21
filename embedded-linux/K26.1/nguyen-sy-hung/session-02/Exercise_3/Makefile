CC = gcc

CFLAGS = -Iinclude

SRC = $(wildcard src/*.c)

OBJ = $(patsubst src/%.c,obj/%.o,$(filter-out src/main.c,$(SRC)))

all: libs bin/main


libs: static shared


static: obj/calc.o
	mkdir -p lib
	ar rcs lib/libcalc.a obj/calc.o


shared: obj/logger.o
	mkdir -p lib
	$(CC) -shared -o lib/liblogger.so obj/logger.o


obj/%.o: src/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -fPIC -c $< -o $@


bin/main: src/main.c libs
	mkdir -p bin
	$(CC) src/main.c $(CFLAGS) -Llib -lcalc -llogger -o bin/main


run: all
	LD_LIBRARY_PATH=./lib ./bin/main


clean:
	rm -rf obj lib bin app.log


rebuild: clean all