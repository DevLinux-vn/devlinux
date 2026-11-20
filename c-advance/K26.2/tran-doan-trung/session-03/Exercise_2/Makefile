CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c11 -g

TARGET = Exercise_2
OBJS = main.o
SRCS = main.c

.PHONY: all clean check

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

check:
	cppcheck --enable=all --suppress=missingIncludeSystem --inconclusive --std=c11 $(SRCS)
	clang-tidy $(SRCS) -checks='clang-analyzer-*,cert-*,bugprone-*' -- $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)