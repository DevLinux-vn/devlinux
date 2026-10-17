CC = gcc
CFLAGS = -Wall -Iinclude

LIB = bin/libstringutils.so
OBJ = build/stringutils.o
TARGET = bin/main

.PHONY: all run clean

all: $(TARGET)

$(OBJ): src/stringutils.c include/stringutils.h
	@mkdir -p build
	$(CC) $(CFLAGS) -fPIC -c src/stringutils.c -o $(OBJ)

$(LIB): $(OBJ)
	@mkdir -p bin
	$(CC) -shared -o $(LIB) $(OBJ)

$(TARGET): main.c $(LIB)
	$(CC) $(CFLAGS) main.c -Lbin -lstringutils -Wl,-rpath,'$$ORIGIN' -o $(TARGET)

run: $(TARGET)
	./$(TARGET) "Hello World"

clean:
	rm -rf build bin
