CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99



TARGET := ../build/app/unit_test
INCS := -I../include -I../tools/unity
SRCS := $(wildcard ../src/*.c)\
	   $(wildcard ../tools/unity/*.c)

TESTSRCS := $(wildcard *.c)

BUILDDIR := ../build/obj

OBJS := $(SRCS:%.c=%.o)
OBJS := $(patsubst ../%, $(BUILDDIR)/%, $(OBJS))
OBJS += $(patsubst %.c, $(BUILDDIR)/test/%.o, $(TESTSRCS))



.PHONY: clean all unittest



all: $(TARGET)
	@echo "Building unit test application ......."

$(TARGET): $(OBJS)
	@echo "Linking unit test application..."
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) -o $@



$(BUILDDIR)/test/%.o: %.c
	@echo "Building objects for unit test"
	@mkdir -p $(dir $@)
	$(CC) $(INCS) $(CFLAGS) -c $< -o $@



$(BUILDDIR)/%.o: ../%.c
	@echo "Building objects for unit test"
	@mkdir -p $(dir $@)
	$(CC) $(INCS) $(CFLAGS) -c $< -o $@


clean:
	@rm -rf $(TARGET) ../build


unittest: all
	./$(TARGET)












# if you want to include relative path in .c example : in test_ipv4_parser.c you include "../src/ipv4_parser.c" then you can use this command to compile
# test_run: test_ipv4_parser.c test_main.c ../src/ipv4_parser.c ../tools/unity/unity.c
# 	@echo "Compiling test files..."
# 	$(CC) $(CFLAGS) $(INCS) $^ -o $(TARGET)
# 	@echo "Running unit tests..."
# 	$(TARGET)