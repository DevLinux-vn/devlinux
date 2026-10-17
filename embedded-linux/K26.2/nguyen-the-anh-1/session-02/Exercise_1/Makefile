.PHONY: all clean run

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = main
OBJ = obj/mathutils.o
LIB = lib/libmathutils.a

all: $(TARGET)

obj lib:
	mkdir -p $@

$(OBJ): src/mathutils.c include/mathutils.h | obj
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ) | lib
	ar rcs $@ $^

$(TARGET): app/main.c $(LIB)
	$(CC) $(CFLAGS) $< -Llib -lmathutils -o $@

run: all
	./$(TARGET)

clean:
	rm -rf obj lib $(TARGET)
