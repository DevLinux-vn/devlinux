CC := gcc
TARGET := logger
SRC := main.c

CFLAGS := -std=gnu99 -Wall -Wextra -Werror

LOG_LEVEL ?= LOG_LEVEL_DEBUG

.PHONY: all clean debug warning cppcheck clang-tidy

all:
	$(CC) $(CFLAGS) -DLOG_LEVEL_MAX=$(LOG_LEVEL) $(SRC) -o $(TARGET)

debug:
	$(MAKE) clean
	$(MAKE) LOG_LEVEL=LOG_LEVEL_DEBUG

warning:
	$(MAKE) clean
	$(MAKE) LOG_LEVEL=LOG_LEVEL_WARNING

cppcheck:
	cppcheck --enable=all --error-exitcode=1 \
		--suppress=missingIncludeSystem \
		$(SRC)

clang-tidy:
	clang-tidy $(SRC) -- \
		-std=gnu99 \
		-DLOG_LEVEL_MAX=LOG_LEVEL_DEBUG

clean:
	rm -f $(TARGET)