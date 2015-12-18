#!/bin/bash
#------------------------------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------------------------------
CURRENT_DIR=`pwd`
WORK_DIR=$1

UBOOT_DIR=${WORK_DIR}/uboot
distro=jessie

#-------------------------------------------
# u-boot, toolchain, imagegen vars
#-------------------------------------------

CC_DIR="${WORK_DIR}/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux"
CC_URL="https://releases.linaro.org/14.09/components/toolchain/binaries/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux.tar.bz2"

CC_FILE="${CC_DIR}.tar.bz2"
CC="${CC_DIR}/bin/arm-linux-gnueabihf-"

#UBOOT_VERSION=''
UBOOT_VERSION='v2015.10'

BOARD_CONFIG='socfpga_de0_nano_soc_defconfig'
MAKE_CONFIG='u-boot-with-spl-dtb.sfp'

UBOOT_SPLFILE=${UBOOT_DIR}/u-boot-with-spl-dtb.sfp

NCORES=`nproc`


function get_toolchain {
# download linaro cross compiler toolchain
# uses multicore extract (lbzip2)

# extract linaro cross compiler toolchain
if [ ! -d ${CC_DIR} ]; then
    if [ ! -f ${CC_FILE} ]; then
        echo "downloading toolchain"
    	wget -c ${CC_URL}
    fi

    echo "extracting toolchain" 
#	tar xf ${CC_FILE}
	tar --use=lbzip2 -xf ${CC_FILE}
fi
}

function fetch_uboot {
# Fetch uboot

echo "cloning u-boot"
git clone git://git.denx.de/u-boot.git uboot

cd uboot
if [ ! -z "$UBOOT_VERSION" ]
then
    git checkout v2015.10
fi
cd ..
}

function build_uboot {
# compile u-boot + spl
export ARCH=arm
export PATH=$CC_DIR/bin/:$PATH
export CROSS_COMPILE=$CC

echo "compiling u-boot"
make mrproper
make $BOARD_CONFIG
make $MAKE_CONFIG -j$NCORES
}

# run functions

if [ ! -z "$WORK_DIR" ]; then
    cd $WORK_DIR
    get_toolchain
    fetch_uboot
    cd $UBOOT_DIR
    build_uboot
else
    echo no workdir parameter given
fi
echo "#---------------------------------------------------------------------------------- "
echo "#-------------build_uboot.sh Finished --------------------------------- "
echo "#---------------------------------------------------------------------------------- "


