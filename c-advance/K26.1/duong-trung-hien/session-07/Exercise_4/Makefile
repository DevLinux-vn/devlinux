CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
DEF_WIFI := -DCONFIG_WIFI_ENABLED
DEF_ETHE := -DCONFIG_ETHERNET_ENABLED
DEF_BOTH := $(DEF_ETHE) $(DEF_WIFI)

SRC := main.c
INC := -I.

BUILD_DIR := build
TARGET := $(BUILD_DIR)/output
TARGET_WIFI := $(TARGET)_wifi 
TARGET_ETHE := $(TARGET)_ethe 
TARGET_BOTH := $(TARGET)_both 
TARGET_NONE := $(TARGET)_none 

.PHONY: all all_variants clean	\
		run_wifi				\
		run_ethe				\
		run_both				\
		run_none				\
		analyst_wifi			\
		analyst_ethe			\
		analyst_both			\
		analyst_none			\
		help

help:
	@echo "Available targets:"
	@echo "all"
	@echo "all_variants"
	@echo "run_wifi"
	@echo "run_ethe"
	@echo "run_both"
	@echo "run_none"
	@echo "analyst_wifi"
	@echo "analyst_ethe"
	@echo "analyst_both"
	@echo "analyst_none"

all: $(TARGET_WIFI)

all_variants: $(TARGET_WIFI) $(TARGET_ETHE) $(TARGET_BOTH) $(TARGET_NONE)

$(TARGET_WIFI): $(SRC) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) $(SRC) $(DEF_WIFI) -o $@

$(TARGET_ETHE): $(SRC) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) $(SRC) $(DEF_ETHE) -o $@

$(TARGET_BOTH): $(SRC) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) $(SRC) $(DEF_BOTH) -o $@

$(TARGET_NONE): $(SRC) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) $(SRC) -o $@

$(BUILD_DIR):
	mkdir -p $@

run_wifi: $(TARGET_WIFI)
	$(shell pwd)/$<

run_ethe: $(TARGET_ETHE)
	$(shell pwd)/$<

run_both: $(TARGET_BOTH)
	$(shell pwd)/$<

run_none: $(TARGET_NONE)
	$(shell pwd)/$<

analyst_wifi: $(SRC)
	clang-tidy $(SRC) -- $(INC) -std=c99 $(DEF_WIFI)
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $^ $(INC) $(DEF_WIFI)

analyst_ethe: $(SRC)
	clang-tidy $(SRC) -- $(INC) -std=c99 $(DEF_ETHE)
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $^ $(INC) $(DEF_ETHE)

analyst_both: $(SRC)
	clang-tidy $(SRC) -- $(INC) -std=c99 $(DEF_BOTH)
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $^ $(INC) $(DEF_BOTH)

analyst_none: $(SRC)
	clang-tidy $(SRC) -- $(INC) -std=c99
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $^ $(INC)

clean: 
	rm -rf $(BUILD_DIR)


