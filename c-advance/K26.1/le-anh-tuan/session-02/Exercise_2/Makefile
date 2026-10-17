CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := stack_depth_monitor
SRC := main.c

.PHONY: all clean cppcheck clang-tidy

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