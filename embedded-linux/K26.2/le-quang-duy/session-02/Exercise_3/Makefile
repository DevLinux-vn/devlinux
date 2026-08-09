CC = gcc
CFLAGS = -Iinclude -Wall
LDFLAGS = -lm

all: bin/main

# Build object file for Static Library
obj/calc.o: src/calc.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c src/calc.c -o obj/calc.o

# Target Static Library
static: obj/calc.o
	@mkdir -p lib
	ar rcs lib/libcalc.a obj/calc.o

# Build object file for Shared Library
obj/logger.o: src/logger.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -fPIC -c src/logger.c -o obj/logger.o

# Target Shared Library
shared: obj/logger.o
	@mkdir -p lib
	$(CC) -shared -o lib/liblogger.so obj/logger.o

# Target both libraries
libs: static shared

# Build main executable
bin/main: src/main.c libs
	@mkdir -p bin
	$(CC) $(CFLAGS) src/main.c -Llib -lcalc -llogger $(LDFLAGS) -o bin/main

run: all
	@LD_LIBRARY_PATH=./lib ./bin/main

clean:
	rm -rf obj lib bin app.log

rebuild: clean all

.PHONY: all libs static shared run clean rebuild