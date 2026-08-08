CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

OBJ = obj/mathutils.o
LIB = lib/libmathutils.a
TARGET = main

all: $(TARGET)

$(OBJ): src/mathutils.c include/mathutils.h
	$(CC) $(CFLAGS) -c src/mathutils.c -o obj/mathutils.o

$(LIB): $(OBJ)
	ar rcs $(LIB) $(OBJ)

$(TARGET): app/main.c $(LIB)
	$(CC) $(CFLAGS) app/main.c -Llib -lmathutils -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf obj/* lib/* $(TARGET)
