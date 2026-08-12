.PHONY: all clean run

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -Lbin -Wl,-rpath,'$$ORIGIN'

OBJ = build/stringutils.o
LIB = bin/libstringutils.so
TARGET = bin/main

all: $(TARGET)

build bin:
	mkdir -p $@

$(OBJ): src/stringutils.c include/stringutils.h | build
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(LIB): $(OBJ) | bin
	$(CC) -shared -o $@ $^

$(TARGET): main.c $(LIB) | bin
	$(CC) $(CFLAGS) $< -Lbin -lstringutils -Wl,-rpath,'$$ORIGIN' -o $@

run: all
	./$(TARGET) "hello embedded linux"

clean:
	rm -rf build bin
