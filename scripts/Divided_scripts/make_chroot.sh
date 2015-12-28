 
#!/bin/bash

# Script for generating qemu chroot and installing depedencies on host.

#---------------------------------------------------------------------------------#
# Variables
#---------------------------------------------------------------------------------#
CHROOT_DIR=${1}

DISTRO=${2}

VALID_DISTROS="jessie,stretch"


#---------------------------------------------------------------------------------#
# local functions
#---------------------------------------------------------------------------------#

install_chroot_deps() {
    sudo apt-get install qemu binfmt-support qemu-user-static qemu-utils schroot debootstrap
}

check_param() {
#is variable not empty ?
if [ ! -f ${CHROOT_DIR} ]; then
    # exists folder ?
    if [ -d ${CHROOT_DIR} ]; then
#    echo "parameter 1 given"
        echo "rootfs dir seleceted = "${CHROOT_DIR}""
        if [ ! -f ${DISTRO} ]; then
            if ! grep -qvwFf <(sed 's/,/\n/g' <<<${VALID_DISTROS,,}) <(sed 's/,/\n/g' <<<${DISTRO,,})
            then
#                printf '%s\n' "$DISTRO in List"
                echo "Distro = "${DISTRO}""
                return 0
            else
                echo "ERROR: Distro invalid"
                return 1
            fi
        else
            echo "ERROR: parameter 2: distro (ie: jessie) missing"
            return 1
        fi
    else
        echo "ERROR: rootfs folder:--> "${CHROOT_DIR} "does not exist"
        return 1
    fi
else
    echo "ERROR: parameter 1: rootfs folder address missing"
    return 1
fi
}

gen_chroot() {
sudo qemu-debootstrap --arch=armhf --keyring /usr/share/keyrings/debian-archive-keyring.gpg \
  --variant=buildd include=git,u-boot-tools,locales,device-tree-compiler,bc --exclude=debfoster $DISTRO $CHROOT_DIR http://ftp.debian.org/debian
}

print_param() {
echo "param 1= "$CHROOT_DIR" param2= "$DISTRO""
}

#---------------------------------------------------------------------------------#
#-------------------------+++++   Run functions   +++++---------------------------#
#---------------------------------------------------------------------------------#
set -e

if [ ! check_param ]; then
    install_chroot_deps
    gen_chroot
fi
