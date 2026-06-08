CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = main
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)
DATA_FILE = products.dat

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) $(DATA_FILE)

rebuild: clean all
