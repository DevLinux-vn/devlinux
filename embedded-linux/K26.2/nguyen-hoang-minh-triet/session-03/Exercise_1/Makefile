# define compiler
CC = gcc

#define compiler flags
CFLAGS = -Wall -Wextra -Werror -std=c99

#define target
TARGET = student_manager

#define source files
SRC = main.c

#define phony targets
.PHONY: all clean

#make target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) students.dat

