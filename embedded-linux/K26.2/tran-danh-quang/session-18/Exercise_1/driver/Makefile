# Linux 6.1 API. Set KDIR to the matching prepared BSP/SDK kernel build tree.
ifneq ($(KERNELRELEASE),)
obj-m += ssd1306_i2c_driver.o
else
.DEFAULT_GOAL := all
MODULE_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
KDIR ?=
.PHONY: all clean
all:
	@test -n "$(KDIR)" || { echo "Set KDIR to a prepared Linux 6.1 kernel build tree" >&2; exit 1; }
	$(MAKE) -C "$(KDIR)" M="$(MODULE_DIR)" modules

clean:
	$(RM) ssd1306_i2c_driver.o ssd1306_i2c_driver.ko ssd1306_i2c_driver.mod
	$(RM) ssd1306_i2c_driver.mod.c ssd1306_i2c_driver.mod.o Module.symvers modules.order
	$(RM) .*.cmd .*.d
	rm -rf .tmp_versions
endif
