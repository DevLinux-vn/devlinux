
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

TARGET = union

cppcheck:
	cppcheck --enable=all --std=c99 --suppress=missingIncludeSystem --suppress=knownConditionTrueFalse main.c
all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -f $(TARGET)