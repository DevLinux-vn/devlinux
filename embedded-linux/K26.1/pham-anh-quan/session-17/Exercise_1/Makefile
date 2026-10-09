obj-m += main.o

KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules
	dtc -I dts -O dtb -o led-blink.dtbo led-blink.dts

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f led-blink.dtbo