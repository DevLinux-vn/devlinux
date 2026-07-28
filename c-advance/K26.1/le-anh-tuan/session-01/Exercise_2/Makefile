CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

.PHONY: all clean cppcheck clang-tidy

all: final

final: main.o
	@echo "Building final application."
	$(CC) $(CFLAGS) main.o -o final
	chmod +x final

main.o: main.c
	@echo "Compiling main file."
	$(CC) $(CFLAGS) -c main.c

clean:
	@echo "Removing process file"
	rm main.o final main

cppcheck:
	@echo "Using cppcheck:"
	cppcheck --enable=all --std=c99 --suppress=missingIncludeSystem main.c

clang-tidy:
	@echo "Using clang-tidy:"
	clang-tidy main.c -checks='clang-analyzer-*,bugprone-*,readability-*' -- $(CFLAGS)
