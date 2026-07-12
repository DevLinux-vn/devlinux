CC := gcc

TARGET := display_demo
BUILD_DIR := build
INCLUDE_DIR := include
SRC_DIR := src

SOURCES := main.c \
           $(SRC_DIR)/console_display.c \
           $(SRC_DIR)/dummy_display.c

OBJECTS := $(BUILD_DIR)/main.o \
           $(BUILD_DIR)/console_display.o \
           $(BUILD_DIR)/dummy_display.o

CFLAGS := -std=c99 \
          -Wall \
          -Wextra \
          -Werror \
          -pedantic \
          -I$(INCLUDE_DIR)

CPPFLAGS := -I$(INCLUDE_DIR)

.PHONY: all clean run cppcheck clang-tidy

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR)/main.o: main.c \
                            $(INCLUDE_DIR)/i_display.h \
                            $(INCLUDE_DIR)/console_display.h \
                            $(INCLUDE_DIR)/dummy_display.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c main.c -o $@

$(BUILD_DIR)/console_display.o: $(SRC_DIR)/console_display.c \
                                       $(INCLUDE_DIR)/console_display.h \
                                       $(INCLUDE_DIR)/i_display.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/console_display.c -o $@

$(BUILD_DIR)/dummy_display.o: $(SRC_DIR)/dummy_display.c \
                                     $(INCLUDE_DIR)/dummy_display.h \
                                     $(INCLUDE_DIR)/i_display.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/dummy_display.c -o $@

run: all
	./$(TARGET)

cppcheck:
	cppcheck --enable=all \
	         --std=c99 \
	         --language=c \
	         --suppress=missingIncludeSystem \
	         --error-exitcode=1 \
	         $(CPPFLAGS) \
	         main.c $(SRC_DIR)

clang-tidy:
	clang-tidy $(SOURCES) -- $(CFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)