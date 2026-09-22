CFLAGS := -Iinclude -fPIC

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, obj/%.o, $(SRCS))

all: create_dirs static shared bin/main

create_dirs:
	mkdir -p obj lib bin

obj/%.o: src/%.c | create_dirs
	gcc $(CFLAGS) -c $< -o $@

static: obj/calc.o | create_dirs
	ar rcs lib/libcalc.a obj/calc.o

shared: obj/logger.o | create_dirs
	gcc -shared -o lib/liblogger.so obj/logger.o

bin/main: obj/main.o static shared | create_dirs
	gcc obj/main.o -Llib -lcalc -llogger -lm -o bin/main

run: all
	LD_LIBRARY_PATH=./lib ldd ./bin/main

clean:
	rm -rf obj/ lib/ bin/ app.log

rebuild: clean all