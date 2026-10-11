CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := token_stringification
SRC := main.c
INC := -I.

.PHONY: all clean run verify clang_tidy cpp_check

all: $(TARGET)

verify: $(SRC)
	$(CC) -E $^

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INC) $^ $(DEF) -o $(TARGET)

run: $(TARGET)
	$(shell pwd)/$<

clang_tidy: $(SRC)
	clang-tidy $^ -checks='clang-analyzer-*,cert-*,bugprone-*' -- -std=c99

cpp_check: $(SRC)
	cppcheck --enable=all --inconclusive --std=c99 --language=c $(INC) --suppress=missingIncludeSystem $^

clean:
	rm -f $(TARGET)