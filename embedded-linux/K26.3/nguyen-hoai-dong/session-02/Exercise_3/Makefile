all: obj bin lib
	gcc -c src/calc.c -o obj/calc.o
	ar rcs lib/libcalc.a obj/calc.o

	gcc -fPIC -c src/logger.c -o obj/logger.o
	gcc -shared -o bin/liblogger.so obj/logger.o

	gcc src/main.c -Iinclude -Llib -Lbin -lcalc -llogger -o bin/main

static: obj
	gcc -c src/calc.c -o obj/calc.o
	ar rcs lib/libcalc.a obj/calc.o
shared: obj
	gcc -fPIC -c src/logger.c -o obj/logger.o
	gcc -shared -o bin/liblogger.so obj/logger.o

run: ./bin/main ./bin/liblogger.so app.log
	LD_LIBRARY_PATH=./bin ./bin/main

rebuild: clean all

clean:
	rm -rf ./obj *.o
	rm -rf ./bin *.so
	rm -rf ./lib *.a
	rm -rf ./bin main
bin:
	mkdir -p bin
obj:
	mkdir -p obj
lib:
	mkdir -p lib
app.log:
	touch app.log