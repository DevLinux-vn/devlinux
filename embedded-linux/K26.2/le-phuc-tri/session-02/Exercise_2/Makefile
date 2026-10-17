CC=gcc

CFLAGS=-Iinclude -fPIC

LIB=bin/libstringutils.so

OBJ=build/stringutils.o

TARGET=bin/main

all: $(TARGET)

$(OBJ): src/stringutils.c include/stringutils.h
	mkdir -p build
	$(CC) $(CFLAGS) -c src/stringutils.c -o $(OBJ)

$(LIB): $(OBJ)
	mkdir -p bin
	$(CC) -shared -o $(LIB) $(OBJ)

$(TARGET): app/main.c $(LIB)
	$(CC) app/main.c -Iinclude -Lbin -lstringutils -Wl,-rpath,'$$ORIGIN' -o $(TARGET)

run: $(TARGET)
	./bin/main Trile1708

clean:
	rm -rf build bin
