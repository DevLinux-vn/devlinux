CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -Llib -lmathutils

all: main

main: lib/libmathutils.a app/main.c
	$(CC) app/main.c $(CFLAGS) $(LDFLAGS) -o main

lib/libmathutils.a: obj/mathutils.o | lib
	ar rcs $@ $<

obj/mathutils.o: src/mathutils.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj

lib:
	mkdir -p lib

run: all
	./main

clean:
	rm -rf obj lib main

.PHONY: all run clean
