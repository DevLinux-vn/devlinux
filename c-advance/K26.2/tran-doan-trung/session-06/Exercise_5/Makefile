CC ?= gcc
CFLAGS ?= -Wall -Wextra -pedantic -Werror -std=c99 -g

TARGET = exercise_5
SRCS = main.c wifi_fsm.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean run check

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

check:
	cppcheck --enable=all --suppress=missingIncludeSystem --inconclusive --std=c99 $(SRCS)
	clang-tidy $(SRCS) -checks='clang-analyzer-*,cert-*,bugprone-*,-bugprone-easily-swappable-parameters' -- $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
