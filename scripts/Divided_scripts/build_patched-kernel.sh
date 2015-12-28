#!/bin/bash

#-----------------------------------------------------------------------------#
# Variables
#-----------------------------------------------------------------------------#
CURRENT_DIR=`pwd`
WORK_DIR=$1

#KERNEL_URL='https://github.com/altera-opensource/linux-socfpga.git'
KERNEL_FILE_URL='ftp://ftp.kernel.org/pub/linux/kernel/v4.x/linux-4.1.15.tar.xz'
KERNEL_FILE="linux-4.1.15.tar.xz"
KERNEL_FOLDER_NAME="linux-4.1.15"

PATCH_URL="https://www.kernel.org/pub/linux/kernel/projects/rt/4.1/patch-4.1.15-rt17.patch.xz"
PATCH_FILE="patch-4.1.15-rt17.patch.xz"
#KERNEL_URL='https://github.com/RobertCNelson/armv7-multiplatform'
KERNEL_CHKOUT='linux-rt linux/socfpga-3.10-ltsi-rt'
#KERNEL_CHKOUT='origin/v4.4.x'

KERNEL_CONF='socfpga_defconfig'

distro=jessie

#CC_DIR="${WORK_DIR}/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux"
#CC="${CC_DIR}/bin/arm-linux-gnueabihf-"
CC="arm-linux-gnueabihf-"


IMG_FILE=${WORK_DIR}/mksoc_sdcard.img
DRIVE=/dev/loop0

KERNEL_DIR=${WORK_DIR}/$KERNEL_FOLDER_NAME

NCORES=`nproc`

function install_dep {
# install deps for kernel build
#sudo apt-get install bc u-boot-tools
sudo apt install sudo bc
}

function fetch_kernel {
    if [ -d ${KERNEL_DIR} ]; then
        echo the kernel target directory $KERNEL_DIR already exists.
#        echo cleaning repo
#        cd $KERNEL_DIR 
#        git clean -d -f -x
    else
        cd $WORK_DIR
        wget $KERNEL_FILE_URL
        wget $PATCH_URL
#        mkdir -p $KERNEL_DIR
#        git clone $KERNEL_URL linux
        tar xf $KERNEL_FILE
#        cd linux 
#        git remote add linux $KERNEL_URL
    fi
#    git fetch linux
#    git checkout -b $KERNEL_CHKOUT

}

patch_kernel() {
cd $KERNEL_DIR
xzcat ../$PATCH_FILE | patch -p1
}

function build_kernel {
export CROSS_COMPILE=$CC
#mkdir -p $KERNEL_DIR/41kernel/$KERNEL_FOLDER_NAME/build
cd $KERNEL_DIR
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

echo "#--------------------------------------------------------------------------#"
echo "#---------------+++        build_kernel.sh Start        +++----------------#"
echo "#--------------------------------------------------------------------------#"
set -e -x
if [ ! -z "$WORK_DIR" ]; then
    if [ ! -d ${KERNEL_DIR} ]; then
        echo installing dependencies
        install_dep
        echo downloading kernel
        fetch_kernel
    else 
        echo onboard kernel found
    fi
    echo "Applying patch"
    patch_kernel
    build_kernel
    echo "#--------------------------------------------------------------------------#"
    echo "#----+++       build_kernel.sh Finished Successfull      +++---------------#"
    echo "#--------------------------------------------------------------------------#"

else
    echo "#--------------------------------------------------------------------------#"
    echo "#---------    build_kernel.sh  Unsuccessfull     --------------------------#"
    echo "#---------    workdir parameter missing    --------------------------------#"
    echo "#--------------------------------------------------------------------------#"
fi


