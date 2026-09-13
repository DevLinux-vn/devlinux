
CC = gcc
AR = ar
ARFLAGS = rcs
CFLAGS = -I include
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)

# Separate object lists
CALC_OBJ = obj/calc.o
LOGGER_OBJ = obj/logger.o

CALC_LIB = lib/libcalc.a
LOGGER_LIB = lib/liblogger.so
TARGET = bin/main

all: $(TARGET)

run:
	./bin/main

obj:
	mkdir -p obj

lib:
	mkdir -p lib

bin:
	mkdir -p bin

SRCS := $(wildcard ./src/*.c)
OBJS := $(SRCS:.c=.o)

static:$(CALC_LIB)
shared:$(LOGGER_LIB)

# Pattern rule: default object compilation
obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

# Special rule for logger.o (needs -fPIC)
obj/logger.o: src/logger.c | obj
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# Archive library (static)
$(CALC_LIB): $(CALC_OBJ) | lib
	$(AR) $(ARFLAGS) $@ $^

# Shared library
$(LOGGER_LIB): $(LOGGER_OBJ) | lib
	$(CC) -shared -o $@ $^

$(TARGET): obj/main.o $(CALC_LIB) $(LOGGER_LIB) | bin
	$(CC) $(CFLAGS) obj/main.o -Llib -lcalc -llogger -o $@


clean:
	rm -r obj/
	rm -r lib/
	rm -r bin/

rebuild: clean all

verify:
	cat app.log
	nm lib/libcalc.a
	ldd bin/main
.PHONY: all libs static shared run clean rebuild
