
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

TARGET = perm_tester
# knownConditionTrueFalse is suppressed because main() uses hardcoded
# demo values to test has_permission(); in production this warning
# would correctly re-appear for dynamic inputs.
cppcheck:
	cppcheck --enable=all --std=c99 --suppress=missingIncludeSystem --suppress=knownConditionTrueFalse main.c
all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -f $(TARGET)