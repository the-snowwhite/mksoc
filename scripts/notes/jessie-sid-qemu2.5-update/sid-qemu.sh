#!/bin/bash

echo "Scripted DEmo description of konsole commands used to update QEMU to 2.5 sid version om my exact Debian Jessie installation"
echo "Not for generic use"

exit

set x
mkdir -p qemu-down && cd qemu-down

URL_LIST='[

http://ftp.dk.debian.org/debian/pool/main/n/nettle/libnettle6_3.1.1-4_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-utils_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/d/debootstrap/debootstrap_1.0.75_all.deb
http://ftp.dk.debian.org/debian/pool/main/b/binfmt-support/binfmt-support_2.1.5-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-user-static_2.5+dfsg-1_amd64.deb

http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-x86_2.5+dfsg-1_amd64.deb

http://ftp.dk.debian.org/debian/pool/main/libc/libcacard/libcacard0_2.5.0-2_amd64.deb

http://ftp.dk.debian.org/debian/pool/main/n/ncurses/libtinfo5_6.0+20151024-2_i386.deb
http://ftp.dk.debian.org/debian/pool/main/n/ncurses/libtinfo5_6.0+20151024-2_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/n/ncurses/libtinfo5_6.0+20151024-2_armhf.deb

http://ftp.dk.debian.org/debian/pool/main/n/ncurses/libtinfo-dev_6.0+20151024-2_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/n/ncurses/libtinfo-dev_6.0+20151024-2_armhf.deb
http://ftp.dk.debian.org/debian/pool/main/n/ncurses/libtinfo-dev_6.0+20151024-2_i386.deb

http://ftp.dk.debian.org/debian/pool/main/x/xen/libxen-4.6_4.6.0-1_amd64.deb

http://ftp.dk.debian.org/debian/pool/main/n/ncurses/libncurses5-dev_6.0+20151024-2_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/n/ncurses/libncurses5-dev_6.0+20151024-2_i386.deb
http://ftp.dk.debian.org/debian/pool/main/n/ncurses/libncursesw5_6.0+20151024-2_amd64.deb

http://ftp.dk.debian.org/debian/pool/main/s/slof/qemu-slof_20150429+dfsg-1_all.deb
http://ftp.dk.debian.org/debian/pool/main/o/openbios/openbios-ppc_1.1+svn1340-1_all.deb
http://ftp.dk.debian.org/debian/pool/main/o/openhackware/openhackware_0.4.1+git-20140423.c559da7c-2_all.deb

wget http://ftp.dk.debian.org/debian/pool/main/o/openbios/openbios-sparc_1.1+svn1340-1_all.deb
wget http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-sparc_2.5+dfsg-1_amd64.deb

wget http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-misc_2.5+dfsg-1_amd64.deb

http://ftp.dk.debian.org/debian/pool/main/s/spice/libspice-server-dev_0.12.6-4_amd64.deb

http://ftp.dk.debian.org/debian/pool/main/o/openssl/libssl-dev_1.0.2e-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/o/openssl/libssl-dev_1.0.2e-1_i386.deb

http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-arm_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-mips_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-ppc_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-sparc_2.5+dfsg-1_arm64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-misc_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-x86_2.5+dfsg-1_arm64.deb

http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-user-binfmt_2.5+dfsg-1_amd64.deb


http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-user_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu_2.5+dfsg-1_amd64.deb

http://ftp.dk.debian.org/debian/pool/main/s/schroot/schroot_1.6.10-2_amd64.deb
]'

# uninstall / purge depedencies:

sudo apt-get purge libncurses5-dev libncurses5* libncursesw5 libtinfo* libxen* libspice-server1* libssl1* libtinfo-dev 


sudo dpkg -P qemu-*
sudo dpkg -P qemu
sudo dpkg -P qemu-user-static
sudo dpkg -P qemu-system
sudo dpkg -P qemu-utils
sudo dpkg -P qemu-user-binfmt
sudo dpkg -P qemu-system-ppc
sudo dpkg -P libssl1.0.0_1.0.2d-1_amd64.deb libssl1.0.0_1.0.2d-1_i386.deb
sudo dpkg -P libssl1 libssl
sudo dpkg -P libssl1
sudo dpkg -P libssl1.0.0
sudo dpkg -P debootstrap
sudo dpkg -P schroot
sudo dpkg -P binfmt-support
sudo dpkg -P schroot

sudo apt-get autoremove
sudo apt-get update
sudo apt-get upgrade

# download packets from sid repo:

wget -i $URL_LIST
URL_LIST2=($URL_LIST)
filelist=''

# create install packet name list:

for file in "${URL_LIST2[@]}"
do
  filename="${file##*/}"
    if [[ ! $filename == *"["* ]] &&  [[ ! $filename == *"]"* ]]; then
        filelist=$filelist" "$filename
    fi
done

# install packets
sudo dpkg -P ${filelist}
echo ${filelist}


