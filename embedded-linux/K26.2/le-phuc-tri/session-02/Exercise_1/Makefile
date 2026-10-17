CC = gcc
CFLAGS = -Iinclude
OBJ = obj/mathutils.o
LIB = lib/libmathutils.a
TARGET = main
all: $(TARGET)
$(OBJ): src/mathutils.c include/mathutils.h
	mkdir -p obj
	$(CC) -c src/mathutils.c $(CFLAGS) -o $(OBJ)
$(LIB): $(OBJ)
	mkdir -p lib
	ar rcs $(LIB) $(OBJ)

$(TARGET): app/main.c $(LIB)
	$(CC) app/main.c $(CFLAGS) -Llib -lmathutils -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf obj lib $(TARGET)

