#!/bin/bash

# untested & incomplete raw alpha template

TODO:   add u-boot

# 1. make minimal rootfs on amd64 Debian Jessie, according to "How to create bare minimum Debian Wheezy rootfs from scratch"
# http://olimex.wordpress.com/2014/07/21/how-to-create-bare-minimum-debian-wheezy-rootfs-from-scratch/

sudo apt-get install qemu-user-static debootstrap binfmt-support
targetdir=~/sd-build/arm-linux-gnueabifh-rootfs
distro=jessie
 
mkdir $targetdir
sudo debootstrap --arch=armhf --foreign $distro $targetdir
 
sudo cp /usr/bin/qemu-arm-static $targetdir/usr/bin/
sudo cp /etc/resolv.conf $targetdir/etc

sudo chroot $targetdir

distro=jessie
export LANG=C
 
/debootstrap/debootstrap --second-stage

cat < /etc/apt/sources.list
deb http://ftp.jp.debian.org/debian $distro main contrib non-free
deb-src http://ftp.jp.debian.org/debian $distro main contrib non-free
deb http://ftp.jp.debian.org/debian $distro-updates main contrib non-free
deb-src http://ftp.jp.debian.org/debian $distro-updates main contrib non-free
deb http://security.debian.org/debian-security $distro/updates main contrib non-free
deb-src http://security.debian.org/debian-security $distro/updates main contrib non-free
EOT

apt-get update
apt-get install locales dialog
dpkg-reconfigure locales

apt-get install openssh-server ntpdate

passwd
 
echo <> /etc/network/interfaces
allow-hotplug eth0
iface eth0 inet static
	address 192.168.1.254
	netmask 255.255.255.248
	gateway 192.168.1.1	
EOT

echo mksoc > /etc/hostname

echo T0:2345:respawn:/sbin/getty -L ttyS0 115200 vt100 >> /etc/inittab
 
exit

# 2. Cross compile Linux kernel

# Reference: http://www.rocketboards.org/foswiki/Documentation/HowUseTftpDebugKernel

kerneldir=~/sd-build/arm-linux-gnueabifh-kernel
ncores = 'nproc'

sudo apt-get install bc u-boot-tools

cd $kerneldir
git clone https://github.com/altera-opensource/linux-socfpga.git rocketboards
cd rocketboards 
git remote add rocketboards https://github.com/altera-opensource/linux-socfpga.git


# git remote show rocketboards
git fetch rocketboards
git checkout -b linux-rt rocketboards/socfpga-3.10-ltsi-rt


cd $kerneldir/rocketboards

make mrproper


make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$ncores socfpga_defconfig 2>&1 | tee ../linux-config-log.txt
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$ncores LOADADDR=0x8000 zImage 2>&1 | tee ../linux-zImage-log.txt
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$ncores modules 2>&1 | tee ../linux-modules-log.txt
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$ncores LOADADDR=0x8000 all 2>&1 | tee ../linux-all-log.txt

# Mount SDCard, which created from Ubuntu image downloaded from Macnica-america's web site. It's not necessary to start with Ubuntu image though just make it easy to create two partitions on SDCard as first fat device, then second ext3 linux fs.

# Create and mount SD-Card image:

IMG_FILE=~/sd-build/mksoc_sdcard.img

DRIVE=/dev/loop0
 
sudo dd if=/dev/zero of=$IMG_FILE bs=3700M count=1

sudo losetup --show -f $IMG_FILE

 
SIZE=`fdisk -l $DRIVE | grep Disk | awk '{print $5}'`
 
echo DISK SIZE - $SIZE bytes
 
CYLINDERS=`echo $SIZE/255/63/512 | bc`
 
echo CYLINDERS - $CYLINDERS
 
{
echo ,9,0x0C,*
echo ,,,-
} | sfdisk -D -H 255 -S 63 -C $CYLINDERS $DRIVE
 
mkfs.vfat -F 32 -n "boot" ${DRIVE}1
mke2fs -j -L "rootfs" ${DRIVE}2


sudo partprobe $DRIVE

sudo mkdir -p /mnt/boot
sudo mount -o uid=1000,gid=1000 ${DRIVE}1 /mnt/boot

sudo cp arch/arm/boot/zImage /mnt/boot
sudo cp arch/arm/boot/dts/socfpga_cyclone5_socdk.dts /mnt/boot/socfpga.dts
sudo umount /mnt/boot

sudo mkdir -p /mnt/rootfs
#sudo mkfs.ext3 /dev/sdb2
sudo mount ${DRIVE}2 /mnt/rootfs
cd $targetdir
sudo tar cf - . | (sudo tar xvf - -C /mnt/rootfs)
cd $kerneldir/rocketboards
sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$ncores LOADADDR=0x8000 modules_install INSTALL_MOD_PATH=/mnt/rootfs

sudo umount /mnt/rootfs
