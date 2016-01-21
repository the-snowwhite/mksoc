obj-m := hmreg.o

KERNEL_SRC_DIR=/home/mib/Development/Projects/arm-linux-gnueabifh-kernel/linux
PWD=$(shell pwd)
CROSS_C="/home/mib/Development/Projects/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-"
//NCORES=`nproc`
NCORES=1

all:
	make -j$(NCORES) ARCH=arm CROSS_COMPILE=$(CROSS_C) -C $(KERNEL_SRC_DIR) M=$(PWD) modules

clean:
	make -j$(NCORES) ARCH=arm CROSS_COMPILE=$(CROSS_C) -C $(KERNEL_SRC_DIR) M=$(PWD) clean
