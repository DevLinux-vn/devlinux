CC = gcc

CFLAGS = -Wall -Wextra -Iinclude

LIB_NAME = lib/libmathutils.a

APP = main

SRC = src/mathutils.c
OBJ = obj/mathutils.o

MAIN_SRC = app/main.c
MAIN_OBJ = obj/main.o

all: $(APP)

# Build static library
$(LIB_NAME): $(OBJ)
	@mkdir -p lib
	ar rcs $(LIB_NAME) $(OBJ)

# Build executable
$(APP): $(LIB_NAME) $(MAIN_OBJ)
	$(CC) $(MAIN_OBJ) -Llib -lmathutils -o $(APP)

# Compile library source
obj/mathutils.o: src/mathutils.c include/mathutils.h
	@mkdir -p obj
	$(CC) $(CFLAGS) -c src/mathutils.c -o obj/mathutils.o

# Compile main source
obj/main.o: app/main.c include/mathutils.h
	@mkdir -p obj
	$(CC) $(CFLAGS) -c app/main.c -o obj/main.o

run: all
	./$(APP)

clean:
	rm -rf obj lib $(APP)

.PHONY: all run clean