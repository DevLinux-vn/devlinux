CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -Lbin -Wl,-rpath,'$$ORIGIN' -lstringutils

OBJ = build/stringutils.o
LIB = bin/libstringutils.so
TARGET = bin/main

all: $(TARGET)

$(OBJ): src/stringutils.c include/stringutils.h
	$(CC) $(CFLAGS) -fPIC -c src/stringutils.c -o $(OBJ)

$(LIB): $(OBJ)
	$(CC) -shared -o $(LIB) $(OBJ)

$(TARGET): main.c $(LIB)
	$(CC) $(CFLAGS) main.c $(LDFLAGS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) "Hello Linux"

clean:
	rm -rf build/* bin/*

.PHONY: all clean run
