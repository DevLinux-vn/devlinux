CC = gcc 
CFLAGS += -Wall -Wextra -pedantic -Werror -std=c99 
TARGET = main
OBJS = main.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) $(OBJS)