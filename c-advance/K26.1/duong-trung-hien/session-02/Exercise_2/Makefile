CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -Iinc

SRC = src/main.c src/stack_depth_monitor.c

TARGET = output 

all: $(TARGET)

$(TARGET) : $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -rf output
	