.DEFAULT_GOAL := all
EXERCISE_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
KDIR ?=

.PHONY: all app driver clean
all: app
ifneq ($(strip $(KDIR)),)
all: driver
else
	@echo "User-space app built. Kernel module requires a prepared Linux 6.1 tree: make KDIR=/path/to/kernel-build"
endif

app:
	$(MAKE) -C "$(EXERCISE_DIR)/app" all

driver:
	$(MAKE) -C "$(EXERCISE_DIR)/driver" all

clean:
	$(MAKE) -C "$(EXERCISE_DIR)/driver" clean
	$(MAKE) -C "$(EXERCISE_DIR)/app" clean
