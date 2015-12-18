#!/bin/bash

#------------------------------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------------------------------
CURRENT_DIR=`pwd`
WORK_DIR=$1

KERNEL_URL='https://github.com/altera-opensource/linux-socfpga.git'
KERNEL_VERSION=socfpga-3.10-ltsi-rt
KERNEL_CONF='socfpga_defconfig'

distro=jessie

CC_DIR="${CURRENT_DIR}/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux"
CC="${CC_DIR}/bin/arm-linux-gnueabihf-"


IMG_FILE=${CURRENT_DIR}/mksoc_sdcard.img
DRIVE=/dev/loop0

KERNEL_DIR=${WORK_DIR}/arm-linux-gnueabifh-kernel

NCORES=`nproc`

function install_dep {
# install deps for kernel build
sudo apt-get install bc u-boot-tools
}

function fetch_kernel {
export CROSS_COMPILE=$CC

mkdir -p $KERNEL_DIR
cd $KERNEL_DIR
git clone $KERNEL_URL linux
cd linux 
git remote add linux $KERNEL_URL


# git remote show linux
git fetch linux
git checkout -b linux-rt linux/$KERNEL_VERSION
}

function build_kernel {
cd $KERNEL_DIR/linux
#clean
make mrproper
# configure
make ARCH=arm $KERNEL_CONF 2>&1 | tee ../linux-config_rt-log.txt

# zImage:
make -j$NCORES ARCH=arm 2>&1 | tee ../linux-make_rt-log_.txt

# modules:
make -j$NCORES ARCH=arm modules 2>&1 | tee ../linux-modules_rt-log.txt
}

#sudo make ARCH=arm INSTALL_MOD_PATH=/mnt/rootfs modules_install

#make ARCH=arm -j$NCORES LOADADDR=0x8000 all 2>&1 | tee ../linux-all-log.txt

if [ ! -z "$WORK_DIR" ]; then
    if [ ! -d ${KERNEL_DIR} ]; then
        echo downloading kernel
        fetch_kernel
    else 
        echo onboard kernel found
    fi

build_kernel
echo "#---------------------------------------------------------------------------------- "
echo "#--------            build_kernel.sh Finished Success           ------------------- "
echo "#---------------------------------------------------------------------------------- "

else
    echo "#---------------------------------------------------------------------------------- "
    echo "#-------------    build_kernel.sh  Unsuccessfull     ------------------------------ "
    echo "#-------------    workdir parameter missing    ------------------------------------ "
    echo "#---------------------------------------------------------------------------------- "
fi


