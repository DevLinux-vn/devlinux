all: main

dirs:
	mkdir -p obj lib

static: lib/libcalc.a

shared: lib/liblogger.so

libs: static shared

obj/calc.o: src/calc.c | dirs
	gcc -Iinclude -c src/calc.c -o obj/calc.o

lib/libcalc.a: obj/calc.o | dirs
	ar rcs lib/libcalc.a obj/calc.o

obj/logger.o: src/logger.c | dirs
	gcc -Iinclude -fPIC -c src/logger.c -o obj/logger.o

lib/liblogger.so: obj/logger.o | dirs
	gcc -shared -o lib/liblogger.so obj/logger.o

main: src/main.c lib/liblogger.so lib/libcalc.a | dirs
	gcc src/main.c -Iinclude -Llib -llogger -lm -lcalc -o main -Wl,-rpath=./lib

run: main
	./main

clean:
	rm -rf obj lib main app.log

rebuild: clean all
