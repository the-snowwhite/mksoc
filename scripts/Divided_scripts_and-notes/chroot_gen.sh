#!/bin/bash

CURRENT_DIR=`pwd`
CHROOT_DIR=$CURRENT_DIR/rootfs

distro=jessie

function run_bootstrap {
sudo qemu-debootstrap --arch=armhf --variant=buildd  --keyring /usr/share/keyrings/debian-archive-keyring.gpg --include=debian-archive-keyring,debian-keyring,debian-ports-archive-keyring,apt-utils,ssh,sudo,ntpdate,openssl,vim,nano,cryptsetup,lvm2,locales,build-essential,gcc,g++,gdb,make,subversion,git,curl,zip,unzip,pbzip2,pigz,dialog,openssh-server,ntpdate,less,cpufrequtils,isc-dhcp-client,ntp,console-setup,ca-certificates,xserver-xorg,xserver-xorg-video-dummy,netbase,iproute2,iputils-ping,iputils-arping,iputils-tracepath,haveged $distro $CHROOT_DIR http://ftp.debian.org/debian/
}

function gen_policy-rc.d {
sudo sh -c 'cat <<EOT > '$CHROOT_DIR'/usr/sbin/policy-rc.d
echo "************************************" >&2
echo "All rc.d operations denied by policy" >&2
echo "************************************" >&2
exit 101
EOT'
}

function edit_apt-get-sources-list  {
sudo sh -c 'cat <<EOT > '$CHROOT_DIR'/etc/apt/sources.list
deb http://ftp.dk.debian.org/debian '$distro'  main contrib non-free
deb-src http://ftp.dk.debian.org/debian  '$distro' main contrib non-free
deb http://ftp.dk.debian.org/debian '$distro'-updates main contrib non-free
deb-src http://ftp.dk.debian.org/debian/ '$distro'-updates main contrib non-free
deb http://security.debian.org/ '$distro'/updates main contrib non-free
deb-src http://security.debian.org '$distro'/updates main contrib non-free
EOT'
}

function gen_initial_sh {
sudo sh -c 'cat <<EOT > '$CHROOT_DIR'/home/initial.sh
#!/bin/bash

sudo mount -t proc proc /proc

DEFGROUPS="sudo,kmem,adm,dialout,machinekit,video,plugdev"
export LANG=C


sudo apt-get -y update
#sudo apt-get -y upgrade
sudo apt-get -y install xorg

sudo locale-gen

echo "NOTE: " "Will add user machinekit pw: machinekit"
sudo /usr/sbin/useradd -s /bin/bash -d /home/machinekit -m machinekit
sudo bash -c "echo "machinekit:machinekit" | chpasswd"
sudo adduser machinekit sudo
sudo chsh -s /bin/bash machinekit

echo "NOTE: ""User Added"

echo "NOTE: ""Will now add user to groups"
sudo usermod -a -G $DEFGROUPS machinekit
sync

echo "NOTE: ""Will now run apt update, upgrade"
sudo apt-get -y update
#sudo apt-get -y upgrade
sudo umount /proc
exit
EOT'

sudo chmod +x $CHROOT_DIR/home/initial.sh
}

function run_initial_sh {
sudo chroot $CHROOT_DIR /bin/bash -c /home/initial.sh
sudo chroot $CHROOT_DIR rm /usr/sbin/policy-rc.d
}

#run_bootstrap
#gen_policy-rc.d
#edit_apt-get-sources-list
gen_initial_sh
run_initial_sh

