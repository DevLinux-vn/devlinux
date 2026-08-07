CC := gcc

CFLAGS := -std=c99 -Wall -Wextra -pedantic -Werror

TARGET := exercise_1

SOURCES := main.c \
           console_display.c \
           dummy_display.c

OBJECTS := $(SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)