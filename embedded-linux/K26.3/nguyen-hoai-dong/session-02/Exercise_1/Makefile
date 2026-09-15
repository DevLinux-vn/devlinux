all: obj lib 
	gcc -c src/mathutils.c -o obj/mathutils.o
	ar rcs lib/libmathutils.a obj/mathutils.o
	gcc app/main.c -Iinclude -Llib -lmathutils -o main
run: main
	./main
clean:
	rm -rf ./obj *.o
	rm -rf ./lib *.a
	rm -rf  main
obj:
	mkdir -p obj
lib:
	mkdir -p lib