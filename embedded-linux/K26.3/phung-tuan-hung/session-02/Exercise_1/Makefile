# Makefile - Exercise_1: static library libmathutils.a
# Targets: all, run, clean

CC       := gcc
CFLAGS   := -Wall -Wextra -Iinclude

OBJDIR   := obj
LIBDIR   := lib

OBJ      := $(OBJDIR)/mathutils.o
LIB      := $(LIBDIR)/libmathutils.a
EXE      := main

# Default target
all: $(EXE)

# 1) Compile library source into an object file
$(OBJ): src/mathutils.c include/mathutils.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c src/mathutils.c -o $(OBJ)

# 2) Archive object file into a static library
$(LIB): $(OBJ) | $(LIBDIR)
	ar rcs $(LIB) $(OBJ)

# 3) Compile main and statically link against libmathutils.a
$(EXE): app/main.c $(LIB)
	$(CC) $(CFLAGS) app/main.c -L$(LIBDIR) -lmathutils -o $(EXE)

# Create output directories if missing
$(OBJDIR):
	mkdir -p $(OBJDIR)
$(LIBDIR):
	mkdir -p $(LIBDIR)

# Run the program
run: $(EXE)
	./$(EXE)

# Remove obj/, lib/, and the executable
clean:
	rm -rf $(OBJDIR) $(LIBDIR) $(EXE)

.PHONY: all run clean
