#!/bin/bash

echo "Very unfinshed not usable in current state"
exit

set x
mkdir -p qemu-down && cd qemu-down

URL_LIST='[
http://ftp.dk.debian.org/debian/pool/main/n/nettle/libnettle6_3.1.1-4_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-utils_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/d/debootstrap/debootstrap_1.0.75_all.deb
http://ftp.dk.debian.org/debian/pool/main/b/binfmt-support/binfmt-support_2.1.5-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-user-static_2.5+dfsg-1_amd64.deb

http://ftp.dk.debian.org/debian/pool/main/libc/libcacard/libcacard0_2.5.0-2_amd64.deb


http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-arm_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-mips_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-ppc_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-sparc_2.5+dfsg-1_arm64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-misc_2.5+dfsg-1_amd64.deb
http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system-x86_2.5+dfsg-1_arm64.deb


http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-system_2.5+dfsg-1_amd64.deb

http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu_2.5+dfsg-1_amd64.deb

http://ftp.dk.debian.org/debian/pool/main/q/qemu/qemu-user-binfmt_2.5+dfsg-1_amd64.deb


http://ftp.dk.debian.org/debian/pool/main/s/schroot/schroot_1.6.10-2_amd64.deb
]'

#wget -i $URL_LIST
URL_LIST2=($URL_LIST)
filelist=''

for file in "${URL_LIST2[@]}"
do
  filename="${file##*/}"
    if [[ ! $filename == *"["* ]] &&  [[ ! $filename == *"]"* ]]; then
        filelist=$filelist" "$filename
    fi
done

#¤sudo dpkg -P ${filelist}
echo ${filelist}


