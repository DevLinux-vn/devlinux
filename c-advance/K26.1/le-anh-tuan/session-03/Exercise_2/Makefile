CC := gcc
TARGET := main
SRC := main.c

CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

.PHONY: all clean run cppcheck clang-tidy check

all: $(TARGET)

$(TARGET): $(SRC)
	@echo "Compiling files"
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

cppcheck:
	@echo "Running cppcheck"
	cppcheck --enable=warning,style,performance,portability \
	         --std=c99 \
	         --error-exitcode=1 \
	         --suppress=missingIncludeSystem \
	         $(SRC)

clang-tidy:
	@echo "Running clang-tidy"
	clang-tidy $(SRC) \
	           -checks='clang-analyzer-*,bugprone-*,readability-*' \
	           --warnings-as-errors='*' \
	           -- $(CFLAGS)	

clean:
	@echo "Cleaning up"
	rm -f $(TARGET)