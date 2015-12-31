#!/bin/bash

# Create, partition and mount SD-Card image:
# parameter 1 --> path to folder where image is generated (required)
# parameter 2 --> name of single partition rootfs image that will be generated instead of full sd img.
#------------------------------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------------------------------
CURRENT_DIR=`pwd`
WORK_DIR=$1
ROOTFS_NAME=$2

SD_IMG=${WORK_DIR}/mksoc_sdcard.img
ROOTFS_IMG=${WORK_DIR}/$ROOTFS_NAME
DRIVE=/dev/loop0

function create_sdcard_img {
#--------------- Initial sd-card image - partitioned --------------
echo "#-------------------------------------------------------------------------------#"
echo "#-----------------------------          ----------------------------------------#"
echo "#---------------     +++ generating sd-card image  zzz  +++ ........  ----------#"
echo "#---------------------------  Please  wait   -----------------------------------#"
echo "#-------------------------------------------------------------------------------#"
sudo dd if=/dev/zero of=$SD_IMG  bs=4096 count=925K

LOOP_MNT=`bash -c 'sudo losetup --show -f '$SD_IMG''`
sudo fdisk $LOOP_MNT << EOF
n
p
3

+1M
t
a2
n
p
2

+3600M
n
p
1


t
1
b
w
EOF

sudo partprobe $LOOP_MNT

echo "creating file systems"

sudo mkfs.vfat -F 32 -n "BOOT" ${LOOP_MNT}p1
sudo mke2fs -j -L "rootfs" ${LOOP_MNT}p2

sync
sudo partprobe $LOOP_MNT
sync
sudo losetup -D
sync
}

function create_rootfs_img {
echo "#-------------------------------------------------------------------------------#"
echo "#-----------------------------          ----------------------------------------#"
echo "#----------------     +++ generating rootfs image  zzz  +++ ........  ----------#"
echo "#-----------------------------   wait   ----------------------------------------#"
echo "#-------------------------------------------------------------------------------#"
sudo dd if=/dev/zero of=$ROOTFS_IMG  bs=2048 count=1801K

LOOP_MNT=`bash -c 'sudo losetup --show -f '$ROOTFS_IMG''`
sudo fdisk $LOOP_MNT << EOF
n
p
1

+3600M
a
w
EOF

sudo partprobe $LOOP_MNT

echo "creating file system"

#sudo mkfs.vfat -F 32 -n "BOOT" ${DRIVE}p1
sudo mke2fs -j -L "rootfs" ${DRIVE}p1

sync
sudo partprobe $DRIVE
sync
sudo losetup -D
sync

}

gen_img() {
if [ -z "$ROOTFS_NAME" ]; then
    if [ -f ${SD_IMG} ]; then
        echo "Deleting old imagefile"
        rm -f $SD_IMG
    fi
create_sdcard_img
else
    if [ -f ${ROOTFS_IMG} ]; then
        echo "Deleting old imagefile"
        rm -f $ROOTFS_IMG
    fi
create_rootfs_img
fi
}


if [ ! -z "$WORK_DIR" ]; then
    gen_img
    echo "#---------------------------------------------------------------------------------- "
    echo "#-------------  create_img.sh Finished with Success  ------------------------------ "
    echo "#---------------------------------------------------------------------------------- "
else
    echo "#---------------------------------------------------------------------------------- "
    echo "#----------------  create_img.sh Ended Unsuccessfully    -------------------------- "
    echo "#-------------  workdir parameter missing     ------------------------------------- "
    echo "#---------------------------------------------------------------------------------- "
fi


