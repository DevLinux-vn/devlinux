CC = gcc
TARGET = endian_checker
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

.PHONY: all clean cppcheck tidy

all: $(TARGET)

$(TARGET): main.c
	@echo "Compiling $(TARGET)"
	$(CC) $(CFLAGS) main.c -o $(TARGET)

cppcheck:
	@echo "Running cppcheck"
	cppcheck --enable=all --std=c99 --suppress=missingIncludeSystem main.c

clang-tidy:
	@echo "Running clang-tidy"
	clang-tidy main.c -checks='clang-analyzer-*,bugprone-*,readability-*' -- $(CFLAGS)

clean:
	@echo "Cleaning up"
	rm -f $(TARGET)