#!/bin/bash

#------------------------------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------------------------------
CURRENT_DIR=`pwd`
WORK_DIR=$1

KERNEL_URL='https://github.com/altera-opensource/linux-socfpga.git'
#KERNEL_URL='https://github.com/RobertCNelson/armv7-multiplatform'
KERNEL_CHKOUT='linux-rt linux/socfpga-3.10-ltsi-rt'
#KERNEL_CHKOUT='origin/v4.4.x'

KERNEL_CONF='socfpga_defconfig'

distro=jessie

CC_DIR="${WORK_DIR}/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux"
CC="${CC_DIR}/bin/arm-linux-gnueabihf-"
#CC="arm-linux-gnueabihf-"


IMG_FILE=${WORK_DIR}/mksoc_sdcard.img
DRIVE=/dev/loop0

KERNEL_BUILD_DIR=${WORK_DIR}/arm-linux-gnueabifh-kernel

NCORES=`nproc`

function install_dep {
# install deps for kernel build
sudo apt-get install bc u-boot-tools 
# install linaro gcc 4.9 crosstoolchain dependency:
sudo apt-get install lib32stdc++6

}

function fetch_kernel {
    if [ -d ${KERNEL_BUILD_DIR} ]; then
        echo the kernel target directory $KERNEL_BUILD_DIR already exists.
        echo cleaning repo
        cd $KERNEL_BUILD_DIR 
        git clean -d -f -x
    else
        mkdir -p $KERNEL_BUILD_DIR
        cd $KERNEL_BUILD_DIR
        git clone $KERNEL_URL linux
        cd linux 
        git remote add linux $KERNEL_URL
    fi
    git fetch linux
    git checkout -b $KERNEL_CHKOUT
    cd ..  
}

function build_kernel {
export CROSS_COMPILE=$CC
cd $KERNEL_BUILD_DIR/linux
#clean
make -j$NCORES mrproper
# configure
make ARCH=arm $KERNEL_CONF 2>&1 | tee ../linux-config_rt-log.txt
#make $KERNEL_CONF 2>&1 | tee ../linux-config_rt-log.txt

# zImage:
make -j$NCORES ARCH=arm 2>&1 | tee ../linux-make_rt-log_.txt
#make -j$NCORES 2>&1 | tee ../linux-make_rt-log_.txt

# modules:
make -j$NCORES ARCH=arm modules 2>&1 | tee ../linux-modules_rt-log.txt
#make -j$NCORES modules 2>&1 | tee ../linux-modules_rt-log.txt
}

echo "#---------------------------------------------------------------------------------- "
echo "#-------------------+++        build_kernel.sh Start        +++-------------------- "
echo "#---------------------------------------------------------------------------------- "

if [ ! -z "$WORK_DIR" ]; then
    if [ ! -d ${KERNEL_BUILD_DIR} ]; then
        echo downloading kernel
        fetch_kernel
    else 
        echo onboard kernel found
    fi

build_kernel
echo "#---------------------------------------------------------------------------------- "
echo "#--------+++       build_kernel.sh Finished Successfull      +++------------------- "
echo "#---------------------------------------------------------------------------------- "

else
    echo "#---------------------------------------------------------------------------------- "
    echo "#-------------    build_kernel.sh  Unsuccessfull     ------------------------------ "
    echo "#-------------    workdir parameter missing    ------------------------------------ "
    echo "#---------------------------------------------------------------------------------- "
fi


