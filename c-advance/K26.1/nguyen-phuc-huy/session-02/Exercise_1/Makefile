# Makefile for Exercise_1: Memory Segment Analyzer

CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99 -O0

.PHONY: all clean build run verify cppcheck clangtidy

all: build

build: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c

run: build
	./main

clean:
	rm -f main *.o

# Verification commands (run manually after build)
verify:
	@echo "=== size output ==="
	size main
	@echo "=== nm output for globals ==="
	nm main | grep -E 'global_(const|init|uninit)' || echo "Globals not found"


clangtidy: 
	@echo "Running clang-tidy static analysis..."
	clang-tidy main.c -checks='clang-analyzer-*,cert-*,bugprone-*,readability-*' -- 

cppcheck:
	@echo "Running cppcheck static analysis..."
	cppcheck --enable=all --inconclusive --std=c99 --language=c --suppress=missingIncludeSystem main.c