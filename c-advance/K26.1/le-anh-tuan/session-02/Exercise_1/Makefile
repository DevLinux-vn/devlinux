CC := gcc
TARGET := memory_map
SRCS := main.c

CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

.PHONY: all clean cppcheck clang-tidy analyze

all: $(TARGET)

$(TARGET): $(SRCS)
	@echo "Compiling $(TARGET)..."
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

cppcheck:
	@echo "Running cppcheck"
	cppcheck --enable=all --std=c99 --suppress=missingIncludeSystem --error-exitcode=1 $(SRCS)

clang-tidy:
	@echo "Running clang-tidy"
	clang-tidy $(SRCS) -checks='clang-analyzer-*,bugprone-*,readability-*' -- $(CFLAGS)

analyze: cppcheck clang-tidy

clean:
	rm -f $(TARGET)