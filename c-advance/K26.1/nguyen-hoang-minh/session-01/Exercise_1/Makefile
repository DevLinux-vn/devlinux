CC       := gcc
CFLAGS   := -Wall -Wextra -pedantic -Werror -std=c99

CPPCHECK   := /c/Program\ Files/Cppcheck/cppcheck.exe
CLANG_TIDY := /c/Program\ Files/LLVM/bin/clang-tidy.exe

TIDY_CHECKS := clang-diagnostic-*,clang-analyzer-*,bugprone-*,cert-*,misc-*,readability-*

SRC := Session 1 homework1.c
EXE := $(if $(filter Windows_NT,$(OS)),.exe,)
BIN := homework1$(EXE)

.PHONY: all run check cppcheck tidy clean

all: $(BIN)

$(BIN): Session\ 1\ homework1.c
	$(CC) $(CFLAGS) -o $@ "$<"

run: $(BIN)
	./$(BIN)

check: cppcheck tidy

cppcheck:
	$(CPPCHECK) --enable=all --std=c99 --language=c \
		--suppress=missingIncludeSystem --suppress=staticFunction \
		--check-level=exhaustive \
		"$(SRC)"

tidy:
	$(CLANG_TIDY) "$(SRC)" --checks="$(TIDY_CHECKS)" -- -std=c99 -Wall -Wextra -pedantic

clean:
	rm -f homework1 homework1.exe homework1.o

