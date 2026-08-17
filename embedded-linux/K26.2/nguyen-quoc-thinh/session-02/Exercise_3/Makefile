CC = gcc
CFLAGS = -Wall -Iinclude

STATIC_LIB = lib/libcalc.a
SHARED_LIB = lib/liblogger.so
TARGET = bin/main

.PHONY: all libs static shared run clean rebuild

all: libs $(TARGET)

libs: static shared

# ---- static library (libcalc.a) ----
obj/calc.o: src/calc.c include/calc.h
	@mkdir -p obj
	$(CC) $(CFLAGS) -c src/calc.c -o obj/calc.o

static: obj/calc.o
	@mkdir -p lib
	ar rcs $(STATIC_LIB) obj/calc.o

# ---- shared library (liblogger.so) ----
obj/logger.o: src/logger.c include/logger.h
	@mkdir -p obj
	$(CC) $(CFLAGS) -fPIC -c src/logger.c -o obj/logger.o

shared: obj/logger.o
	@mkdir -p lib
	$(CC) -shared -o $(SHARED_LIB) obj/logger.o

# ---- main executable, links both libs ----
$(TARGET): src/main.c $(STATIC_LIB) $(SHARED_LIB)
	@mkdir -p bin
	$(CC) $(CFLAGS) src/main.c -Llib -lcalc -llogger -Wl,-rpath,'$$ORIGIN/../lib' -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -rf obj lib bin app.log

rebuild: clean all
