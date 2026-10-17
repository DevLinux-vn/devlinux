CC = gcc

# This exercise uses -fshort-enums to demonstrate that the compiler
# may reduce the size of an enum to the smallest integer type that can
# represent all its values.
#
# Warning: -fshort-enums changes the ABI. All object files and linked
# libraries must be compiled with the same flag to avoid enum size
# mismatches and incompatible data layouts.
CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -fshort-enums -Iinc

SRC = src/*.c test/*.c

BUILD_DIR = build
TARGET = $(BUILD_DIR)/output

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(BUILD_DIR) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean: 
	rm -rf $(BUILD_DIR)
