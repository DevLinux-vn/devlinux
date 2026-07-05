CC := gcc

CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := advanced_nested_packing
SRC := main.c

.PHONY: all clean cppcheck clang-tidy analyze

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

cppcheck:
	cppcheck --enable=all --std=c99 --suppress=missingIncludeSystem $(SRC)

clang-tidy:
	clang-tidy $(SRC) -- $(CFLAGS)

analyze: cppcheck clang-tidy

clean:
	rm -f $(TARGET)