#!/bin/bash

# script to install machinekit on a clean linux (debian) installation so
# it's easy to start developing for Machinekit and try it out
# future scripts should add the possibility to add a Xenomai kernel and/or
# the QtQuickVCP development prerequisits
# there are no build prerequisits installed
# for more info see https://github.com/machinekit/machinekit/issues/229

# ? first download and install czmq from:
# https://packages.debian.org/source/sid/czmq 
# 
# sudo dpkg -i libczmq3_3.0.2-1_armhf.deb  libczmq-dev_3.0.2-1_armhf.deb
# sudo apt-get install -f

# ? then libwebsockets 1.3

# before running this script, make sure that the user running this script has
# sudo privileges. For instance, if the user is "machinekit" then do:
# $ su
# $ [password]
# visudo
# now add beneath "%root ALL=(ALL:ALL) ALL" the line:
#                 "%machinekit ALL=(ALL:ALL) ALL"
#                       ^---------------------------with correct user name

# ----->   The defaults below are valid for the Debian Wheezy distro
# ----->   please review and edit for other platforms 

# the repository and branch settings are for the machinekit master branch.
# please adapt as needed.

## ----- begin configurable options ---

# this is where the test build will go. 
SCRATCH=${HOME}/machinekit

# git repository to pull from
REPO=git://github.com/machinekit/machinekit.git

# the origin to use
ORIGIN=github-machinekit

# the branch to build
BRANCH=master

# the configure command line.

# for the Beaglebone, use:
#CONFIG_ARGS=" --with-xenomai --with-posix --with-platform=beaglebone"

# for the raspberry, use this:
#CONFIG_ARGS=" --with-xenomai --with-posix --with-platform=raspberry"

# for the development install use:
#CONFIG_ARGS=" --with-posix"

# for the cyclone v soc install use:
#CONFIG_ARGS=" --with-rt-preempt --with-posix --with-extra-kernel-sources=/home/mib/Documents/Altera/WS2/test/rocketboards"
CONFIG_ARGS=" --with-rt-preempt --with-posix"

# echo commands during execution - very verbose
# comment out once you trust this
set -x

# comment out these lines before running this script:
#echo "please review and edit this script before use."
#exit 1

# prerequisits for cloning Machinekit
# git
sudo apt-get install git-core git-gui dpkg-dev

# ----------- end configurable options --------


# refuse to clone into an existing directory.
if [ -d "$SCRATCH" ]; then
    echo the target directory $SCRATCH already exists.
    echo cleaning repo
    cd $SCRATCH 
    git clean -d -f -x
    cd ..    
#    echo please remove or rename this directory and run again.
#    exit 1
else
# $SCRATCH does not exist. Make a shallow git clone into it.
# make sure you have around 200MB free space.

    git clone -b "$BRANCH" -o "$ORIGIN" --depth 1 "$REPO" "$SCRATCH"
fi 

# prerequisits for building from a fresh debian distro
# dependencies + more from https://github.com/mhaberler/asciidoc-sandbox/wiki/Machinekit-Build-for-Multiple-RT-Operating-Systems#installation
sudo apt-get install -y libudev-dev libmodbus-dev libboost-python-dev libusb-1.0-0-dev autoconf pkg-config glib-2.0 gtk+-2.0 tcllib tcl-dev tk-dev bwidget libxaw7-dev libreadline6-dev python-tk libqt4-opengl libqt4-opengl-dev libtk-img python-opengl glade python-xlib python-gtkglext1 python-configobj python-vte libglade2-dev python-glade2 python-gtksourceview2 libncurses-dev libreadline-dev libboost-serialization-dev libboost-thread-dev libjansson-dev lsb-release git dpkg-dev rsyslog automake uuid-runtime ccache  avahi-daemon avahi-discover libnss-mdns bc cython netcat

#sudo sh -c \
#    "echo 'deb http://deb.dovetail-automata.com jessie main' > \
#    /etc/apt/sources.list.d/machinekit.list"
sudo apt-get update
#sudo apt-get install dovetail-automata-keyring

sudo apt-get install -y python-zmq libjansson-dev libwebsockets-dev python-pyftpdlib libzmq3-dev
#cd ${HOME}
#sudo dpkg -i libwebsockets3_1.3-1.deb libwebsockets-dev_1.3-1.deb
#sudo apt-get install -f


#sudo apt-get install python-zmq
sudo apt-get update
wget http://ftp.dk.debian.org/debian/pool/main/c/czmq/libczmq-dev_3.0.2-2_armhf.deb
wget http://ftp.dk.debian.org/debian/pool/main/c/czmq/libczmq3_3.0.2-2_armhf.deb
sudo dpkg -i libczmq3_3.0.2-2_armhf.deb libczmq-dev_3.0.2-2_armhf.deb
#sudo dpkg -i libczmq2_2.2.0-0.5.deb  libczmq-dev_2.2.0-0.5.deb
sudo apt-get -f install

# fail the script on any error
set -e 

# setup ccache:
env CC="ccache gcc" CXX="ccache"

CORES=`nproc`

# make sure some files are in place to finish the build without errors
cd "$SCRATCH/src"
sudo cp ./rtapi/rsyslogd-linuxcnc.conf /etc/rsyslog.d/linuxcnc.conf
sudo touch  /var/log/linuxcnc.log
sudo chmod 644 /var/log/linuxcnc.log
sudo service rsyslog restart
sudo cp ./rtapi/shmdrv/limits.d-machinekit.conf /etc/security/limits.d/linuxcnc.conf
sudo cp ./rtapi/shmdrv/shmdrv.rules /etc/udev/rules.d/50-LINUXCNC-shmdrv.rules

echo installing dependencies 
sudo apt-get install --no-install-recommends devscripts equivs

cd "$SCRATCH"
echo now in directory: `pwd` 

debian/configure -pr 
sudo mk-build-deps -ir

echo building in "$SCRATCH/src"
cd "$SCRATCH/src"
echo now in directory: `pwd` 

# QA: log what just was checked out

# log the origin
# git remote -v

# show the top commit 
# git log -n1

# show that the branch has properly been checked out
# git status

# configure and build
sh autogen.sh

./configure ${CONFIG_ARGS}

make -j $CORES

# Check for sudo
if which sudo >/dev/null 2>&1 ; then
    if sudo -l make setuid >/dev/null 2>&1 ; then
        sudo make setuid
    else
        echo "Cannot run \"sudo make setuid\""
        echo "Please run the following commands as root:"
        echo "  cd $SCRATCH/src"
        echo "  sudo make setuid"
    fi
else
    echo "sudo not found"
    echo "please run the following commands as root:"
    echo "  cd $SCRATCH/src"
    echo "  sudo make setuid"
fi

echo make completed

# check the system configuration (logging etc)

../scripts/check-system-configuration.sh

# no more echo commands during
set +x

echo "looks like the build succeeded!"
echo ""
echo "to run linuxcnc from this build, please execute first:"
echo ". $SCRATCH/scripts/rip-environment" 
echo ""
echo "Please set your name and email for git with:"
echo "git config --global user.name yourname"
echo "git config --global user.email \"youremail\""

