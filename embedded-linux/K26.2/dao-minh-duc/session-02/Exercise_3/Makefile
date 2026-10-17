CC = gcc
CFLAGS = -Iinclude

OBJ = obj/calc.o obj/logger.o

STATIC_LIB = lib/libcalc.a
SHARED_LIB = lib/liblogger.so

TARGET = bin/main

.PHONY: all libs static shared run clean rebuild

all: $(TARGET)

libs: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

obj/calc.o: src/calc.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c src/calc.c -o obj/calc.o

obj/logger.o: src/logger.c
	mkdir -p obj
	$(CC) $(CFLAGS) -fPIC -c src/logger.c -o obj/logger.o

$(STATIC_LIB): obj/calc.o
	mkdir -p lib
	ar rcs $(STATIC_LIB) obj/calc.o

$(SHARED_LIB): obj/logger.o
	mkdir -p lib
	$(CC) -shared -o $(SHARED_LIB) obj/logger.o

$(TARGET): src/main.c $(STATIC_LIB) $(SHARED_LIB)
	mkdir -p bin
	$(CC) $(CFLAGS) src/main.c -Llib -lcalc -llogger -Wl,-rpath,'$$ORIGIN/../lib' -lm -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf obj lib bin app.log

rebuild:
	make clean
	make
