CC = gcc
CFLAGS = -Wall -g -Iinclude
AR = ar
ARFLAGS = rcs

TARGET = main
LIB_NAME = lib/libmathutils.a
OBJ = obj/mathutils.o

all: $(TARGET)

$(TARGET): app/main.c $(LIB_NAME)
	$(CC) $(CFLAGS) app/main.c -Llib -lmathutils -o $@

$(LIB_NAME): $(OBJ) | lib
	$(AR) $(ARFLAGS) $@ $^

$(OBJ): src/mathutils.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj

lib:
	mkdir -p lib

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf obj lib $(TARGET)

.PHONY: all run clean