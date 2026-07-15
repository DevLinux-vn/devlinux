CC = gcc

CFLAGS = -Iinclude

OBJ = obj/mathutils.o

LIB = lib/libmathutils.a

TARGET = main

all: $(TARGET)

$(LIB): $(OBJ)
	ar rcs $(LIB) $(OBJ)

obj/mathutils.o: src/mathutils.c
	$(CC) -c src/mathutils.c $(CFLAGS) -o obj/mathutils.o

$(TARGET): app/main.c $(LIB)
	$(CC) app/main.c $(CFLAGS) -Llib -lmathutils -o $(TARGET)

run:
	./$(TARGET)

clean:
	rm -rf obj/*.o
	rm -rf lib/*.a
	rm -f $(TARGET)