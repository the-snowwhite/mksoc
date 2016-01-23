obj-m := hmreg.o

KERNEL_SRC_DIR=./arm-linux-gnueabifh-kernel/linux
PWD=$(shell pwd)
CROSS_C="/home/mib/Development/hm2reg_module/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-"

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS_C) -C $(KERNEL_SRC_DIR) \
		M=$(PWD) modules

clean:
	make ARCH=arm CROSS_COMPILE=$(CROSS_C) -C $(KERNEL_SRC_DIR) \
		M=$(PWD) clean
