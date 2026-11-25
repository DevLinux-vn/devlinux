# External-module API baseline: upstream Linux 6.1.
# Build only against an explicitly selected, prepared target tree:
#   make KDIR=/path/to/target/build ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
ifneq ($(KERNELRELEASE),)
obj-m += led_blink.o
led_blink-y := main.o
else
.DEFAULT_GOAL := all
MODULE_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
KDIR ?=
DTC ?= dtc
DT_INCLUDE ?= $(KDIR)/include

.PHONY: all clean overlay
all:
	@test -n "$(KDIR)" || { echo "Set KDIR to the prepared target kernel build tree" >&2; exit 1; }
	$(MAKE) -C "$(KDIR)" M="$(MODULE_DIR)" modules

# cpp resolves GPIO_ACTIVE_HIGH; -@ retains external gpio symbol fixups.
overlay: led-blink.dtbo

led-blink.dtbo: led-blink.dts $(DT_INCLUDE)/dt-bindings/gpio/gpio.h
	$(CPP) -nostdinc -undef -D__DTS__ -x assembler-with-cpp -I "$(DT_INCLUDE)" "$<" -o led-blink.pre.dts
	$(DTC) -@ -I dts -O dtb -o "$@" led-blink.pre.dts

clean:
	$(RM) main.o led_blink.o led_blink.ko led_blink.mod led_blink.mod.c led_blink.mod.o
	$(RM) Module.symvers modules.order .*.cmd .*.d led-blink.pre.dts led-blink.dtbo
	rm -rf .tmp_versions
endif
