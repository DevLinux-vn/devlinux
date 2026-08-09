CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = main
LIB = lib/libmathutils.a
OBJ = obj/mathutils.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): app/main.c $(LIB)
	$(CC) $(CFLAGS) app/main.c -Llib -lmathutils -o $(TARGET)

$(LIB): $(OBJ)
	mkdir -p lib
	ar rcs $(LIB) $(OBJ)

$(OBJ): src/mathutils.c include/mathutils.h
	mkdir -p obj
	$(CC) $(CFLAGS) -c src/mathutils.c -o $(OBJ)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf obj lib $(TARGET)