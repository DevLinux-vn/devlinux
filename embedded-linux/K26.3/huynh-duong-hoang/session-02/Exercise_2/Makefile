CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS = -Lbin -lstringutils -Wl,-rpath,'$$ORIGIN'

TARGET = bin/main
SHARED_LIB = bin/libstringutils.so
OBJ = build/stringutils.o

all: $(TARGET)

$(TARGET): main.c $(SHARED_LIB) | bin
	$(CC) $(CFLAGS) main.c $(LDFLAGS) -o $@

$(SHARED_LIB): $(OBJ) | bin
	$(CC) -shared -o $@ $<

$(OBJ): src/stringutils.c | build
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

build:
	mkdir -p build

bin:
	mkdir -p bin

run: $(TARGET)
	./$(TARGET) "Hello Embedded Linux"

clean:
	rm -rf build bin

.PHONY: all run clean