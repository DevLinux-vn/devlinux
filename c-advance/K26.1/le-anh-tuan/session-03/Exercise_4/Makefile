CC := gcc
TARGET := main
SRC := main.c

CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

.PHONY: all clean cppcheck clang-tidy check

all: $(TARGET)

$(TARGET): $(SRC)
	@echo "Compiling files"
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

cppcheck:
	@echo "Running cppcheck"
	cppcheck --enable=all --std=c99 --suppress=missingIncludeSystem $(SRC)

clang-tidy:
	@echo "Running clang-tidy"
	clang-tidy $(SRC) -checks='clang-analyzer-*,bugprone-*,readability-*' -- $(CFLAGS)

clean:
	@echo "Cleaning up"
	rm -f $(TARGET)