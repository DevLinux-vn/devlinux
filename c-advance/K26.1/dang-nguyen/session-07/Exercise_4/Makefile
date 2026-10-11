CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := conditional_compilation
TARGET_WIFI := $(TARGET)_wifi
TARGET_ETHE := $(TARGET)_eth
TARGET_BOTH := $(TARGET)_both
TARGET_NONE := $(TARGET)_none
SRC := main.c
INC := -I.
DEF_WIFI := -DCONFIG_WIFI_ENABLED
DEF_ETHE := -DCONFIG_ETHERNET_ENABLED
DEF_BOTH := $(DEF_WIFI) $(DEF_ETHE)

.PHONY: all all_variants clean	\
		$(TARGET_BOTH)			\
		$(TARGET_WIFI)			\
		$(TARGET_ETHE)			\
		$(TARGET_NONE)			\
		run_wifi				\
		run_ethe				\
		run_both				\
		run_none				\
		static_analysis_wifi	\
		static_analysis_ethe	\
		static_analysis_both	\
		static_analysis_none

all: $(TARGET_WIFI)

all_variants: $(TARGET_BOTH) $(TARGET_WIFI) $(TARGET_ETHE) $(TARGET_NONE)

$(TARGET_WIFI): $(SRC)
	$(CC) $(CFLAGS) $(INC) $^ $(DEF_WIFI) -o $@

$(TARGET_ETHE): $(SRC)
	$(CC) $(CFLAGS) $(INC) $^ $(DEF_ETHE) -o $@

$(TARGET_BOTH): $(SRC)
	$(CC) $(CFLAGS) $(INC) $^ $(DEF_BOTH) -o $@

$(TARGET_NONE): $(SRC)
	$(CC) $(CFLAGS) $(INC) $^ -o $@

run_wifi: $(TARGET_WIFI)
	$(shell pwd)/$<

run_ethe: $(TARGET_ETHE)
	$(shell pwd)/$<

run_both: $(TARGET_BOTH)
	$(shell pwd)/$<

run_none: $(TARGET_NONE)
	$(shell pwd)/$<

static_analysis_wifi: $(SRC)
	clang-tidy $^ -checks='clang-analyzer-*,cert-*,bugprone-*' -- -std=c99 $(DEF_WIFI)
	cppcheck --enable=all --inconclusive --std=c99 --language=c $(INC) $(DEF_WIFI) --suppress=missingIncludeSystem $^

static_analysis_ethe: $(SRC)
	clang-tidy $^ -checks='clang-analyzer-*,cert-*,bugprone-*' -- -std=c99 $(DEF_ETHE)
	cppcheck --enable=all --inconclusive --std=c99 --language=c $(INC) $(DEF_ETHE) --suppress=missingIncludeSystem $^

static_analysis_both: $(SRC)
	clang-tidy $^ -checks='clang-analyzer-*,cert-*,bugprone-*' -- -std=c99 $(DEF_BOTH)
	cppcheck --enable=all --inconclusive --std=c99 --language=c $(INC) $(DEF_BOTH) --suppress=missingIncludeSystem $^

static_analysis_none: $(SRC)
	clang-tidy $^ -checks='clang-analyzer-*,cert-*,bugprone-*' -- -std=c99
	cppcheck --enable=all --inconclusive --std=c99 --language=c $(INC) --suppress=missingIncludeSystem $^

clean:
	rm -f $(TARGET_BOTH) $(TARGET_WIFI) $(TARGET_ETHE) $(TARGET_NONE)