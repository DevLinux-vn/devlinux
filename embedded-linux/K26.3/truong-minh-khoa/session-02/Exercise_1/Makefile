CC = gcc
AR = ar
ARFLAGS = rcs
CFLAGS=-Wall -Wextra -std=c99 -Iinclude

all: main

run:
	./main


obj:
	mkdir -p obj

lib:
	mkdir -p lib

mathutil: obj
	$(CC) $(CFLAGS) -c src/mathutils.c -o obj/mathutils.o

mathutil_archive: mathutil lib
	$(AR) $(ARFLAGS) lib/libmathutils.a obj/mathutils.o

main:mathutil_archive
	$(CC) $(CFLAGS) app/main.c -Llib -lmathutils -o main


.PHONY: clean run all
clean:
	rm -r obj/
	rm -r lib/
	rm main
