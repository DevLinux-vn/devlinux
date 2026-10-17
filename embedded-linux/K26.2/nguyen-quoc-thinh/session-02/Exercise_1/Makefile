CC = gcc
CFLAGS = -Wall -Iinclude

LIB = lib/libmathutils.a
OBJ = obj/mathutils.o
TARGET = main

.PHONY: all run clean

all: $(TARGET)

$(OBJ): src/mathutils.c include/mathutils.h
	@mkdir -p obj
	$(CC) $(CFLAGS) -c src/mathutils.c -o $(OBJ)

$(LIB): $(OBJ)
	@mkdir -p lib
	ar rcs $(LIB) $(OBJ)

$(TARGET): app/main.c $(LIB)
	$(CC) $(CFLAGS) app/main.c -Llib -lmathutils -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf obj lib $(TARGET)
