CC      = gcc
CFLAGS  = -Wall -I include

LIB     = lib/libmathutils.a
OBJS    = obj/mathutils.o
MAIN    = main

all: $(MAIN)

# Build static library
$(LIB): $(OBJS) | lib
	ar rcs $@ $^

# Compile library source
obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

# Build executable, link with static library
$(MAIN): app/main.c $(LIB)
	$(CC) $(CFLAGS) $< -L lib -lmathutils -o $@

# Create directories if missing
obj lib:
	mkdir -p $@

run: $(MAIN)
	./$(MAIN)

clean:
	rm -rf obj lib $(MAIN)

.PHONY: all run clean
