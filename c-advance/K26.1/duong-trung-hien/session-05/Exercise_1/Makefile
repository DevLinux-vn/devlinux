CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -Iinc

SRC = src/*.c test/*.c

BUILD_DIR = build

TARGET = $(BUILD_DIR)/output

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(SRC) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

analyze:
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem inc src test -Iinc
	clang-tidy src/*.c test/*.c -- -Iinc -std=c99

clean:
	rm -rf $(BUILD_DIR)

