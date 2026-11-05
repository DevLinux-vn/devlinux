CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET_SECURE := secure
TARGET_VULNERABLE := vulnerable
SRC := main.c
INC := -I.

.PHONY: all clean run clang_tidy cpp_check

all: $(TARGET_SECURE) $(TARGET_VULNERABLE)

$(TARGET_SECURE): $(SRC)
	$(CC) $(CFLAGS) $(INC) $^ -o $@

$(TARGET_VULNERABLE): $(SRC)
	$(CC) -g -fno-stack-protector -DVULNERABLE $^ -o $@

run: $(TARGET_SECURE)
	$(shell pwd)/$<

clang_tidy: $(SRC)
	clang-tidy $^ -checks='clang-analyzer-*,cert-*,bugprone-*' -- -std=c99

cpp_check: $(SRC)
	cppcheck --enable=all --inconclusive --inline-suppr --std=c99 --language=c $(INC) --suppress=missingIncludeSystem $^

clean:
	rm -f $(TARGET_SECURE) $(TARGET_VULNERABLE)