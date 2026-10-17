CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = bin/main
LIB = bin/libstringutils.so
OBJ = build/stringutils.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): app/main.c $(LIB)
	$(CC) $(CFLAGS) app/main.c -Lbin -lstringutils -o $(TARGET)

$(LIB): $(OBJ)
	mkdir -p bin
	$(CC) -shared -o $(LIB) $(OBJ)

$(OBJ): src/stringutils.c include/stringutils.h
	mkdir -p build
	$(CC) $(CFLAGS) -fPIC -c src/stringutils.c -o $(OBJ)

run: $(TARGET)
	LD_LIBRARY_PATH=bin ./$(TARGET) "hello world"

clean:
	rm -rf build bin