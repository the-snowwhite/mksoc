#!/bin/bash




# --> Unused for now
# due to /dev unmount problem

#------------------------------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------------------------------
set -e
WAITTIME=20
CURRENT_DIR=`pwd`
WORK_DIR=$1
MOUNT_DIR=$2

SD_IMG=${WORK_DIR}/mksoc_sdcard.img
ROOTFS_IMG=${WORK_DIR}/rootfs.img
DRIVE=/dev/loop0
ROOTFS_DIR=/mnt/rootfs

distro=jessie

DEFGROUPS="sudo,kmem,adm,dialout,machinekit,video,plugdev"

function make_chroot_init_script {

#sudo LANG=C chroot rootfs /bin/bash exit
sudo sh -c 'cat <<EOT > '$ROOTFS_DIR'/home/initial.sh

export LANG=C


sudo apt-get -y update
sudo apt-get -y upgrade
sudo apt-get -y install xorg

sudo locale-gen

echo "NOTE: " "Will add user machinekit pw: machinekit"
sudo /usr/sbin/useradd -s /bin/bash -d /home/machinekit -m machinekit
sudo bash -c 'echo 'machinekit:machinekit' | chpasswd'
sudo adduser machinekit sudo
sudo chsh -s /bin/bash machinekit

echo "NOTE: ""User Added"

echo "NOTE: ""Will now add user to groups"
sudo usermod -a -G $DEFGROUPS machinekit
sync

echo "NOTE: ""Will now run apt update, upgrade"
sudo apt-get -y update
#sudo apt-get -y upgrade
sync
exit
EOT'
}

function rootfs_img_install {
if [ ! -z "$MOUNT_DIR" ]; then
    ROOTFS_DIR=${MOUNT_DIR}/rootfs
    sudo mkdir -p ${ROOTFS_DIR}
    sudo mount ${ROOTFS_IMG} ${ROOTFS_DIR}
    echo "NOTE: ""rootfs "$ROOTFS_IMG" is mounted in:"
    echo "NOTE: "'rootfs_dir ='$ROOTFS_DIR
    run_func
    sudo umount ${ROOTFS_DIR}
    echo "NOTE: ""rootfs was unounted "
    echo "NOTE: ""rootfs is now in imagefile:"$ROOTFS_IMG
    sync
else
    ROOTFS_DIR=${WORK_DIR}/rootfs
    echo "NOTE: ""no Mountdir parameter given chroot will only be made in current local folder:"   
    echo "NOTE: "'rootfs_dir ='$ROOTFS_DIR 
    run_func
fi

}

function bind_dev {
distro=jessie
export LANG=C

echo "NOTE: ""Will now mount chroot bindings in  "$ROOTFS_DIR"/{dev,proc,dev/pts,sys,tmp}"

#sudo mount -t proc /proc $ROOTFS_DIR/proc
#sudo mount -o bind /dev $ROOTFS_DIR/dev
#sudo mount -o bind /dev/pts $ROOTFS_DIR/dev/pts
#sudo mount -o bind /sys $ROOTFS_DIR/sys

sudo mount -o bind /dev $ROOTFS_DIR/dev
sudo mount -o bind /proc $ROOTFS_DIR/proc
sudo mount -o bind /dev/pts $ROOTFS_DIR/dev/pts
sudo mount -o bind /sys $ROOTFS_DIR/sys
sudo mount -t tmpfs tmpfs  $ROOTFS_DIR/tmp
}

function ubind_dev {
echo "NOTE: ""Will unmount rootfs/{{tmp,sys,dev/pts,proc,dev}"
echo "in "$WAITTIME" sec"
sleep $WAITTIME

#sudo umount $ROOTFS_DIR/{sys,proc,dev/pts,dev}
if ! sudo umount --recursive $ROOTFS_DIR/{tmp,sys,dev/pts,proc,dev}; then
    echo "NOTE: ""Failed to unmount!  Already unmounted?"
else
    echo "NOTE: ""rootfs dev bind unmount successfull"
fi
#sudo umount $ROOTFS_DIR/{tmp,sys,dev/pts,proc,dev}
}
function run_func {
#    install_dep
#    run_bootstrap
#    setup_configfiles
    make_chroot_init_script    
#    run_chroot
}
#----------------------- Run functions ----------------------------#
#run_chroot
rootfs_img_install
