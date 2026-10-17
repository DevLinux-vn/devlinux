# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Target executable name
TARGET = main

# Source files and object files
SRCS = main.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.c signal_handler.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean