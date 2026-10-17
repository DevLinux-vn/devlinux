# Compiler build and flags
CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

# Create a static library for the ipv4_parser module
AR := ar
ARFLAGS := rcs



# Directory
MODULE := ipv4_parser
# result in : src/ipv4_parser.c
LIBSRCS := $(wildcard src/*.c)    
INC := -Iinclude
BUILD_DIR := build
# Same powerful optionOBJS := $(patsubst src/%.c, $(BULD_DIR)/%.O, $(SRCS)) if SRCS := $(wildcard src/*/*.c)
# LIBOBJS := $(LIBSRCS:src/%.c=$(BUILD_DIR)/%.o) 
LIBOBJS := $(patsubst src/%.c, $(BUILD_DIR)/obj/%.o, $(LIBSRCS))     
TARGET := lib/lib$(MODULE).a
LIBDIR := lib
LIB := -L$(LIBDIR) -l$(MODULE)


TESTSRC := main.c
TESTOBJ := $(BUILD_DIR)/obj/main.o
TESTTARGET := $(BUILD_DIR)/app/user_test
# Define clean and all are not a file targets, they are commands
.PHONY: clean all test clangtidy cppcheck

# Default target of this Makefile is static library ipv4_parser
all: $(TARGET)
	@echo "Building static library ......."



# same as mkdir -p $(dir $@)  # $@ is the target file, $< is the first prerequisite if TARGET /lib/driver/.a  dir = lib/drivver
# Create a static library from the object files
$(TARGET): $(LIBOBJS)  
	@echo "Creating static library..."
	@mkdir -p lib                       
	$(AR) $(ARFLAGS) $(TARGET) $(LIBOBJS)
#you change mkdir

# Create object files from the source files -c for compiler and assembler not linker
$(BUILD_DIR)/obj/%.o: src/%.c 
	@echo "Compiling object files for static library..."
	@mkdir -p $(BUILD_DIR)/obj                         
	$(CC) $(INC) $(CFLAGS) -c $< -o $@








# order of linking stage gcc [object files] [libraries] -o output
$(TESTTARGET): $(TESTOBJ) $(TARGET) 
	@echo "Linking test application with static library..."
	@mkdir -p $(BUILD_DIR)/app
	$(CC) $(TESTOBJ) $(LIB) -o $(TESTTARGET) 


# Compile file main
$(TESTOBJ): $(TESTSRC) 
	@echo "Compiling object files for test application..."
	@mkdir -p $(BUILD_DIR)/obj 
	$(CC) $(INC) $(CFLAGS) -c $< -o $@



# Build main test application
test: $(TESTTARGET)
	@echo "Building test application..."


run: test
	@echo "Running test application..."
	./$(TESTTARGET) $(ARGS)


clean: 
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR) $(TESTOBJ) $(TESTTARGET) $(BUILD_DIR) $(TARGET) $(LIBOBJS) $(LIBDIR)


# Analyzer static code in Lib

clangtidy: 
	@echo "Running clang-tidy static analysis..."
	clang-tidy $(LIBSRCS) include/*h -checks='clang-analyzer-*,cert-*,bugprone-*,readability-*' -- -Iinclude

cppcheck:
	@echo "Running cppcheck static analysis..."
	cppcheck --enable=all --inconclusive --std=c99 --language=c -Iinclude --suppress=missingIncludeSystem include/*.h $(LIBSRCS) main.c