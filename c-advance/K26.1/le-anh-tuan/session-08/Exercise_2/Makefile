CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99 -g
TARGET := exercise_2
SRC := main.c

.PHONY: all clean cppcheck clang-tidy check

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

cppcheck:
	cppcheck --enable=all --error-exitcode=1 \
		--suppress=missingIncludeSystem \
		--std=c99 $(SRC)

clang-tidy:
	clang-tidy $(SRC) -- \
		-Wall -Wextra -pedantic -Werror -std=c99

check: all cppcheck clang-tidy

clean:
	rm -f $(TARGET)