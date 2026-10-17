CC      = gcc
CFLAGS  = -Wall -I include -fPIC
LIB     = bin/libstringutils.so
EXE     = bin/app

all: $(EXE)

# Compile library object
build/stringutils.o: src/stringutils.c | build
	 $(CC) $(CFLAGS) -c $< -o $@

# Link shared library
$(LIB): build/stringutils.o | bin
	$(CC) -shared $< -o $@

# Build executable, link to .so
$(EXE): src/main.c $(LIB) | bin
	$(CC) -Wall -I include $< -L bin -lstringutils -Wl,-rpath,'$$ORIGIN' -o $@

build bin:
	mkdir -p $@

run: $(EXE)
	./$(EXE) "Hello Linux"

clean:
	rm -rf build bin

.PHONY: all run clean
