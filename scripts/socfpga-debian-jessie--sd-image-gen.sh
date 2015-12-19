#!/bin/bash

# running(not updated --> see divided folder instead), unfinhed beta template

#TODO:   complete MK depedencies

# 1.initial source: make minimal rootfs on amd64 Debian Jessie, according to "How to create bare minimum Debian Wheezy rootfs from scratch"
# http://olimex.wordpress.com/2014/07/21/how-to-create-bare-minimum-debian-wheezy-rootfs-from-scratch/

#------------------------------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------------------------------
CURRENT_DIR=`pwd`

targetdir=${CURRENT_DIR}/rootfs
distro=jessie

#-------------------------------------------
# u-boot, toolchain, imagegen vars
#-------------------------------------------

CC_DIR="${CURRENT_DIR}/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux"
CC_URL="https://releases.linaro.org/14.09/components/toolchain/binaries/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux.tar.bz2"

CC_FILE="${CC_DIR}.tar.bz2"
CC="${CC_DIR}/bin/arm-linux-gnueabihf-"

#UBOOT_VERSION=''
UBOOT_VERSION='v2015.10'

UBOOT_SPLFILE=${CURRENT_DIR}/uboot/u-boot-with-spl-dtb.sfp

IMG_FILE=${CURRENT_DIR}/mksoc_sdcard.img
DRIVE=/dev/loop0

kerneldir=${CURRENT_DIR}/arm-linux-gnueabifh-kernel

NCORES=`nproc`

#------------------------------------------------------------------------------------------------------
# build armhf Debian qemu-debootstrap chroot port
#------------------------------------------------------------------------------------------------------

sudo apt-get install qemu binfmt-support qemu-user-static schroot debootstrap

# for kernel build
sudo apt-get install bc u-boot-tools

sudo qemu-debootstrap --arch=armhf --variant=buildd  --keyring /usr/share/keyrings/debian-archive-keyring.gpg --include=ssh,sudo,ntpdate,openssl,vim,nano,cryptsetup,lvm2,locales,dialog,openssh-server,ntpdate,less,cpufrequtils,isc-dhcp-server,ntp,console-setup,ca-certificates,xserver-xorg,xserver-xorg-video-dummy,haveged $distro $targetdir http://ftp.debian.org/debian/

sudo sh -c 'echo mksoc > '$targetdir'/etc/hostname'

#127.0.1.1       mksoc.local      mksoc
sudo sh -c 'cat <<EOT > '$targetdir'/etc/hosts

127.0.0.1       localhost
127.0.1.1       mksoc
::1             localhost ip6-localhost ip6-loopback
fe00::0         ip6-localnet
ff00::0         ip6-mcastprefix
ff02::1         ip6-allnodes
ff02::2         ip6-allrouters
EOT'

sudo sh -c 'cat <<EOT > '$targetdir'/etc/apt/sources.list
deb http://ftp.dk.debian.org/debian '$distro'  main contrib non-free
deb-src http://ftp.dk.debian.org/debian  '$distro' main contrib non-free
deb http://ftp.dk.debian.org/debian '$distro'-updates main contrib non-free
deb-src http://ftp.dk.debian.org/debian/ '$distro'-updates main contrib non-free
deb http://security.debian.org/ '$distro'/updates main contrib non-free
deb-src http://security.debian.org '$distro'/updates main contrib non-free
EOT'
 
sudo sh -c 'echo <<EOT >> '$targetdir'/etc/network/interfaces
auto lo eth0
iface lo inet loopback
allow-hotplug eth0
    iface eth0 inet dhcp
EOT'

sudo sh -c 'echo T0:2345:respawn:rootfs/sbin/getty -L ttyS0 115200 vt100 >> '$targetdir'/etc/inittab'

#sudo echo "************************************" >&2
#sudo echo "All rc.d operations denied by policy" >&2
#sudo echo "************************************" >&2
#sudo exit 101

#distro=jessie
#export LANG=C

#DEFGROUPS="sudo,adm,dialout,machinekit,video,plugdev,bluetooth"

sudo sh -c 'cat <<EOT > '$targetdir'/etc/locale.gen
# This file lists locales that you wish to have built. You can find a list
# of valid supported locales at /usr/share/i18n/SUPPORTED, and you can add
# user defined locales to /usr/local/share/i18n/SUPPORTED. If you change
# this file, you need to rerun locale-gen.


# aa_DJ ISO-8859-1
# aa_DJ.UTF-8 UTF-8
# aa_ER UTF-8
# aa_ER@saaho UTF-8
# aa_ET UTF-8
# af_ZA ISO-8859-1
# af_ZA.UTF-8 UTF-8
# ak_GH UTF-8
# am_ET UTF-8
# an_ES ISO-8859-15
# an_ES.UTF-8 UTF-8
# anp_IN UTF-8
# ar_AE ISO-8859-6
# ar_AE.UTF-8 UTF-8
# ar_BH ISO-8859-6
# ar_BH.UTF-8 UTF-8
# ar_DZ ISO-8859-6
# ar_DZ.UTF-8 UTF-8
# ar_EG ISO-8859-6
# ar_EG.UTF-8 UTF-8
# ar_IN UTF-8
# ar_IQ ISO-8859-6
# ar_IQ.UTF-8 UTF-8
# ar_JO ISO-8859-6
# ar_JO.UTF-8 UTF-8
# ar_KW ISO-8859-6
# ar_KW.UTF-8 UTF-8
# ar_LB ISO-8859-6
# ar_LB.UTF-8 UTF-8
# ar_LY ISO-8859-6
# ar_LY.UTF-8 UTF-8
# ar_MA ISO-8859-6
# ar_MA.UTF-8 UTF-8
# ar_OM ISO-8859-6
# ar_OM.UTF-8 UTF-8
# ar_QA ISO-8859-6
# ar_QA.UTF-8 UTF-8
# ar_SA ISO-8859-6
# ar_SA.UTF-8 UTF-8
# ar_SD ISO-8859-6
# ar_SD.UTF-8 UTF-8
# ar_SS UTF-8
# ar_SY ISO-8859-6
# ar_SY.UTF-8 UTF-8
# ar_TN ISO-8859-6
# ar_TN.UTF-8 UTF-8
# ar_YE ISO-8859-6
# ar_YE.UTF-8 UTF-8
# as_IN UTF-8
# ast_ES ISO-8859-15
# ast_ES.UTF-8 UTF-8
# ayc_PE UTF-8
# az_AZ UTF-8
# be_BY CP1251
# be_BY.UTF-8 UTF-8
# be_BY@latin UTF-8
# bem_ZM UTF-8
# ber_DZ UTF-8
# ber_MA UTF-8
# bg_BG CP1251
# bg_BG.UTF-8 UTF-8
# bho_IN UTF-8
# bn_BD UTF-8
# bn_IN UTF-8
# bo_CN UTF-8
# bo_IN UTF-8
# br_FR ISO-8859-1
# br_FR.UTF-8 UTF-8
# br_FR@euro ISO-8859-15
# brx_IN UTF-8
# bs_BA ISO-8859-2
# bs_BA.UTF-8 UTF-8
# byn_ER UTF-8
# ca_AD ISO-8859-15
# ca_AD.UTF-8 UTF-8
# ca_ES ISO-8859-1
# ca_ES.UTF-8 UTF-8
# ca_ES.UTF-8@valencia UTF-8
# ca_ES@euro ISO-8859-15
# ca_ES@valencia ISO-8859-15
# ca_FR ISO-8859-15
# ca_FR.UTF-8 UTF-8
# ca_IT ISO-8859-15
# ca_IT.UTF-8 UTF-8
# cmn_TW UTF-8
# crh_UA UTF-8
# cs_CZ ISO-8859-2
# cs_CZ.UTF-8 UTF-8
# csb_PL UTF-8
# cv_RU UTF-8
# cy_GB ISO-8859-14
# cy_GB.UTF-8 UTF-8
# da_DK ISO-8859-1
# da_DK.UTF-8 UTF-8
# de_AT ISO-8859-1
# de_AT.UTF-8 UTF-8
# de_AT@euro ISO-8859-15
# de_BE ISO-8859-1
# de_BE.UTF-8 UTF-8
# de_BE@euro ISO-8859-15
# de_CH ISO-8859-1
# de_CH.UTF-8 UTF-8
# de_DE ISO-8859-1
# de_DE.UTF-8 UTF-8
# de_DE@euro ISO-8859-15
# de_LI.UTF-8 UTF-8
# de_LU ISO-8859-1
# de_LU.UTF-8 UTF-8
# de_LU@euro ISO-8859-15
# doi_IN UTF-8
# dv_MV UTF-8
# dz_BT UTF-8
# el_CY ISO-8859-7
# el_CY.UTF-8 UTF-8
# el_GR ISO-8859-7
# el_GR.UTF-8 UTF-8
# en_AG UTF-8
# en_AU ISO-8859-1
# en_AU.UTF-8 UTF-8
# en_BW ISO-8859-1
# en_BW.UTF-8 UTF-8
# en_CA ISO-8859-1
# en_CA.UTF-8 UTF-8
# en_DK ISO-8859-1
# en_DK.ISO-8859-15 ISO-8859-15
en_DK.UTF-8 UTF-8
# en_GB ISO-8859-1
# en_GB.ISO-8859-15 ISO-8859-15
en_GB.UTF-8 UTF-8
# en_HK ISO-8859-1
# en_HK.UTF-8 UTF-8
# en_IE ISO-8859-1
# en_IE.UTF-8 UTF-8
# en_IE@euro ISO-8859-15
# en_IN UTF-8
# en_NG UTF-8
# en_NZ ISO-8859-1
# en_NZ.UTF-8 UTF-8
# en_PH ISO-8859-1
# en_PH.UTF-8 UTF-8
# en_SG ISO-8859-1
# en_SG.UTF-8 UTF-8
# en_US ISO-8859-1
# en_US.ISO-8859-15 ISO-8859-15
en_US.UTF-8 UTF-8
# en_ZA ISO-8859-1
# en_ZA.UTF-8 UTF-8
# en_ZM UTF-8
# en_ZW ISO-8859-1
# en_ZW.UTF-8 UTF-8
# eo ISO-8859-3
# eo.UTF-8 UTF-8
# es_AR ISO-8859-1
# es_AR.UTF-8 UTF-8
# es_BO ISO-8859-1
# es_BO.UTF-8 UTF-8
# es_CL ISO-8859-1
# es_CL.UTF-8 UTF-8
# es_CO ISO-8859-1
# es_CO.UTF-8 UTF-8
# es_CR ISO-8859-1
# es_CR.UTF-8 UTF-8
# es_CU UTF-8
# es_DO ISO-8859-1
# es_DO.UTF-8 UTF-8
# es_EC ISO-8859-1
# es_EC.UTF-8 UTF-8
# es_ES ISO-8859-1
# es_ES.UTF-8 UTF-8
# es_ES@euro ISO-8859-15
# es_GT ISO-8859-1
# es_GT.UTF-8 UTF-8
# es_HN ISO-8859-1
# es_HN.UTF-8 UTF-8
# es_MX ISO-8859-1
# es_MX.UTF-8 UTF-8
# es_NI ISO-8859-1
# es_NI.UTF-8 UTF-8
# es_PA ISO-8859-1
# es_PA.UTF-8 UTF-8
# es_PE ISO-8859-1
# es_PE.UTF-8 UTF-8
# es_PR ISO-8859-1
# es_PR.UTF-8 UTF-8
# es_PY ISO-8859-1
# es_PY.UTF-8 UTF-8
# es_SV ISO-8859-1
# es_SV.UTF-8 UTF-8
# es_US ISO-8859-1
# es_US.UTF-8 UTF-8
# es_UY ISO-8859-1
# es_UY.UTF-8 UTF-8
# es_VE ISO-8859-1
# es_VE.UTF-8 UTF-8
# et_EE ISO-8859-1
# et_EE.ISO-8859-15 ISO-8859-15
# et_EE.UTF-8 UTF-8
# eu_ES ISO-8859-1
# eu_ES.UTF-8 UTF-8
# eu_ES@euro ISO-8859-15
# eu_FR ISO-8859-1
# eu_FR.UTF-8 UTF-8
# eu_FR@euro ISO-8859-15
# fa_IR UTF-8
# ff_SN UTF-8
# fi_FI ISO-8859-1
# fi_FI.UTF-8 UTF-8
# fi_FI@euro ISO-8859-15
# fil_PH UTF-8
# fo_FO ISO-8859-1
# fo_FO.UTF-8 UTF-8
# fr_BE ISO-8859-1
# fr_BE.UTF-8 UTF-8
# fr_BE@euro ISO-8859-15
# fr_CA ISO-8859-1
# fr_CA.UTF-8 UTF-8
# fr_CH ISO-8859-1
# fr_CH.UTF-8 UTF-8
# fr_FR ISO-8859-1
# fr_FR.UTF-8 UTF-8
# fr_FR@euro ISO-8859-15
# fr_LU ISO-8859-1
# fr_LU.UTF-8 UTF-8
# fr_LU@euro ISO-8859-15
# fur_IT UTF-8
# fy_DE UTF-8
# fy_NL UTF-8
# ga_IE ISO-8859-1
# ga_IE.UTF-8 UTF-8
# ga_IE@euro ISO-8859-15
# gd_GB ISO-8859-15
# gd_GB.UTF-8 UTF-8
# gez_ER UTF-8
# gez_ER@abegede UTF-8
# gez_ET UTF-8
# gez_ET@abegede UTF-8
# gl_ES ISO-8859-1
# gl_ES.UTF-8 UTF-8
# gl_ES@euro ISO-8859-15
# gu_IN UTF-8
# gv_GB ISO-8859-1
# gv_GB.UTF-8 UTF-8
# ha_NG UTF-8
# hak_TW UTF-8
# he_IL ISO-8859-8
# he_IL.UTF-8 UTF-8
# hi_IN UTF-8
# hne_IN UTF-8
# hr_HR ISO-8859-2
# hr_HR.UTF-8 UTF-8
# hsb_DE ISO-8859-2
# hsb_DE.UTF-8 UTF-8
# ht_HT UTF-8
# hu_HU ISO-8859-2
# hu_HU.UTF-8 UTF-8
# hy_AM UTF-8
# hy_AM.ARMSCII-8 ARMSCII-8
# ia_FR UTF-8
# id_ID ISO-8859-1
# id_ID.UTF-8 UTF-8
# ig_NG UTF-8
# ik_CA UTF-8
# is_IS ISO-8859-1
# is_IS.UTF-8 UTF-8
# it_CH ISO-8859-1
# it_CH.UTF-8 UTF-8
# it_IT ISO-8859-1
# it_IT.UTF-8 UTF-8
# it_IT@euro ISO-8859-15
# iu_CA UTF-8
# iw_IL ISO-8859-8
# iw_IL.UTF-8 UTF-8
# ja_JP.EUC-JP EUC-JP
# ja_JP.UTF-8 UTF-8
# ka_GE GEORGIAN-PS
# ka_GE.UTF-8 UTF-8
# kk_KZ PT154
# kk_KZ RK1048
# kk_KZ.UTF-8 UTF-8
# kl_GL ISO-8859-1
# kl_GL.UTF-8 UTF-8
# km_KH UTF-8
# kn_IN UTF-8
# ko_KR.EUC-KR EUC-KR
# ko_KR.UTF-8 UTF-8
# kok_IN UTF-8
# ks_IN UTF-8
# ks_IN@devanagari UTF-8
# ku_TR ISO-8859-9
# ku_TR.UTF-8 UTF-8
# kw_GB ISO-8859-1
# kw_GB.UTF-8 UTF-8
# ky_KG UTF-8
# lb_LU UTF-8
# lg_UG ISO-8859-10
# lg_UG.UTF-8 UTF-8
# li_BE UTF-8
# li_NL UTF-8
# lij_IT UTF-8
# lo_LA UTF-8
# lt_LT ISO-8859-13
# lt_LT.UTF-8 UTF-8
# lv_LV ISO-8859-13
# lv_LV.UTF-8 UTF-8
# lzh_TW UTF-8
# mag_IN UTF-8
# mai_IN UTF-8
# mg_MG ISO-8859-15
# mg_MG.UTF-8 UTF-8
# mhr_RU UTF-8
# mi_NZ ISO-8859-13
# mi_NZ.UTF-8 UTF-8
# mk_MK ISO-8859-5
# mk_MK.UTF-8 UTF-8
# ml_IN UTF-8
# mn_MN UTF-8
# mni_IN UTF-8
# mr_IN UTF-8
# ms_MY ISO-8859-1
# ms_MY.UTF-8 UTF-8
# mt_MT ISO-8859-3
# mt_MT.UTF-8 UTF-8
# my_MM UTF-8
# nan_TW UTF-8
# nan_TW@latin UTF-8
# nb_NO ISO-8859-1
# nb_NO.UTF-8 UTF-8
# nds_DE UTF-8
# nds_NL UTF-8
# ne_NP UTF-8
# nhn_MX UTF-8
# niu_NU UTF-8
# niu_NZ UTF-8
# nl_AW UTF-8
# nl_BE ISO-8859-1
# nl_BE.UTF-8 UTF-8
# nl_BE@euro ISO-8859-15
# nl_NL ISO-8859-1
# nl_NL.UTF-8 UTF-8
# nl_NL@euro ISO-8859-15
# nn_NO ISO-8859-1
# nn_NO.UTF-8 UTF-8
# nr_ZA UTF-8
# nso_ZA UTF-8
# oc_FR ISO-8859-1
# oc_FR.UTF-8 UTF-8
# om_ET UTF-8
# om_KE ISO-8859-1
# om_KE.UTF-8 UTF-8
# or_IN UTF-8
# os_RU UTF-8
# pa_IN UTF-8
# pa_PK UTF-8
# pap_AN UTF-8
# pap_AW UTF-8
# pap_CW UTF-8
# pl_PL ISO-8859-2
# pl_PL.UTF-8 UTF-8
# ps_AF UTF-8
# pt_BR ISO-8859-1
# pt_BR.UTF-8 UTF-8
# pt_PT ISO-8859-1
# pt_PT.UTF-8 UTF-8
# pt_PT@euro ISO-8859-15
# quz_PE UTF-8
# ro_RO ISO-8859-2
# ro_RO.UTF-8 UTF-8
# ru_RU ISO-8859-5
# ru_RU.CP1251 CP1251
# ru_RU.KOI8-R KOI8-R
# ru_RU.UTF-8 UTF-8
# ru_UA KOI8-U
# ru_UA.UTF-8 UTF-8
# rw_RW UTF-8
# sa_IN UTF-8
# sat_IN UTF-8
# sc_IT UTF-8
# sd_IN UTF-8
# sd_IN@devanagari UTF-8
# se_NO UTF-8
# shs_CA UTF-8
# si_LK UTF-8
# sid_ET UTF-8
# sk_SK ISO-8859-2
# sk_SK.UTF-8 UTF-8
# sl_SI ISO-8859-2
# sl_SI.UTF-8 UTF-8
# so_DJ ISO-8859-1
# so_DJ.UTF-8 UTF-8
# so_ET UTF-8
# so_KE ISO-8859-1
# so_KE.UTF-8 UTF-8
# so_SO ISO-8859-1
# so_SO.UTF-8 UTF-8
# sq_AL ISO-8859-1
# sq_AL.UTF-8 UTF-8
# sq_MK UTF-8
# sr_ME UTF-8
# sr_RS UTF-8
# sr_RS@latin UTF-8
# ss_ZA UTF-8
# st_ZA ISO-8859-1
# st_ZA.UTF-8 UTF-8
# sv_FI ISO-8859-1
# sv_FI.UTF-8 UTF-8
# sv_FI@euro ISO-8859-15
# sv_SE ISO-8859-1
# sv_SE.ISO-8859-15 ISO-8859-15
# sv_SE.UTF-8 UTF-8
# sw_KE UTF-8
# sw_TZ UTF-8
# szl_PL UTF-8
# ta_IN UTF-8
# ta_LK UTF-8
# te_IN UTF-8
# tg_TJ KOI8-T
# tg_TJ.UTF-8 UTF-8
# th_TH TIS-620
# th_TH.UTF-8 UTF-8
# the_NP UTF-8
# ti_ER UTF-8
# ti_ET UTF-8
# tig_ER UTF-8
# tk_TM UTF-8
# tl_PH ISO-8859-1
# tl_PH.UTF-8 UTF-8
# tn_ZA UTF-8
# tr_CY ISO-8859-9
# tr_CY.UTF-8 UTF-8
# tr_TR ISO-8859-9
# tr_TR.UTF-8 UTF-8
# ts_ZA UTF-8
# tt_RU UTF-8
# tt_RU@iqtelif UTF-8
# ug_CN UTF-8
# uk_UA KOI8-U
# uk_UA.UTF-8 UTF-8
# unm_US UTF-8
# ur_IN UTF-8
# ur_PK UTF-8
# uz_UZ ISO-8859-1
# uz_UZ.UTF-8 UTF-8
# uz_UZ@cyrillic UTF-8
# ve_ZA UTF-8
# vi_VN UTF-8
# wa_BE ISO-8859-1
# wa_BE.UTF-8 UTF-8
# wa_BE@euro ISO-8859-15
# wae_CH UTF-8
# wal_ET UTF-8
# wo_SN UTF-8
# xh_ZA ISO-8859-1
# xh_ZA.UTF-8 UTF-8
# yi_US CP1255
# yi_US.UTF-8 UTF-8
# yo_NG UTF-8
# yue_HK UTF-8
# zh_CN GB2312
# zh_CN.GB18030 GB18030
# zh_CN.GBK GBK
# zh_CN.UTF-8 UTF-8
# zh_HK BIG5-HKSCS
# zh_HK.UTF-8 UTF-8
# zh_SG GB2312
# zh_SG.GBK GBK
# zh_SG.UTF-8 UTF-8
# zh_TW BIG5
# zh_TW.EUC-TW EUC-TW
# zh_TW.UTF-8 UTF-8
# zu_ZA ISO-8859-1
# zu_ZA.UTF-8 UTF-8
EOT'

sudo sh -c 'cat <<EOT > '$targetdir'/etc/locale.conf
LANG=en_GB.UTF-8 UTF-8
LC_COLLATE=C
LC_TIME=en_DK.UTF-8
EOT'

sudo sh -c 'cat <<EOT > '$targetdir'/etc/X11/xorg.conf

Section "Screen"
       Identifier   "Default Screen"
       tDevice       "Dummy"
       DefaultDepth 24
EndSection

Section "Device"
    Identifier "Dummy"
    Driver "dummy"
    Option "IgnoreEDID" "true"
    Option "NoDDC" "true"
EndSection
EOT'



echo "Will mount rootfs/{sys,proc,dev/pts,dev}"

sudo mount -o bind /dev $targetdir/dev
sudo mount -o bind /proc $targetdir/proc
sudo mount -o bind /dev/pts $targetdir/dev/pts
sudo mount -o bind /sys $targetdir/sys
sudo mount -t tmpfs tmpfs  $targetdi/tmp

#sudo chroot rootfs /bin/bash exit

sudo chroot $targetdir sudo apt-get -y update
sudo chroot $targetdir sudo apt-get -y upgrade
sudo chroot $targetdir sudo apt-get -y install xorg

sudo chroot $targetdir sudo locale-gen

echo "Will add user machinekit pw: machinekit"
sudo chroot $targetdir sudo /usr/sbin/useradd -s /bin/bash -d /home/machinekit -m machinekit
sudo chroot $targetdir sudo bash -c 'echo 'machinekit:machinekit' | chpasswd'
sudo chroot $targetdir sudo adduser machinekit sudo
sudo chroot $targetdir sudo chsh -s /bin/bash machinekit

echo "User Added"

echo "Will now add user to groups"
sudo chroot $targetdir sudo usermod -a -G "kmem,adm,dialout,machinekit,video,plugdev" machinekit
sync
sudo chroot $targetdir sudo apt-get -y update
sudo chroot $targetdir sudo apt-get -y upgrade

sync

echo "Will unmount rootfs/{sys,proc,dev/pts,dev}"
sudo umount rootfs/{sys,proc,dev/pts,dev,tmp}
#sudo umount rootfs

echo "#---------------------------------------------------------------------------------- "
echo "#----------debian-jessie-armhf-rootfs-gen.sh Part Finished ----------------------- "
echo "#---------------------------------------------------------------------------------- "


###############################################################
# download linaro cross compiler toolchain
# uses multicore extract (lbzip2)

if [ ! -f ${CC_FILE} ]; then
    echo "downloading toolchain"
	wget -c ${CC_URL}
fi

# extract linaro cross compiler toolchain
if [ ! -d ${CC_DIR} ]; then
    echo "extracting toolchain" 
#	tar xf ${CC_FILE}
	tar --use=lbzip2 -xf ${CC_FILE}
fi

###############################################################
# Fetch and compile u-boot + spl

echo "cloning u-boot"
git clone git://git.denx.de/u-boot.git uboot

cd uboot
if [ ! -z "$UBOOT_VERSION" ]
then
    git checkout v2015.10
fi

export ARCH=arm
export PATH=$CC_DIR/bin/:$PATH
export CROSS_COMPILE=$CC

echo "compiling u-boot"
make mrproper
make socfpga_de0_nano_soc_defconfig
make u-boot-with-spl-dtb.sfp -j$NCORES
###############################################################
# Create, partition and mount SD-Card image:


function create_imagefile {
#--------------- Initial sd-card image - partitioned --------------
sudo dd if=/dev/zero of=$IMG_FILE  bs=1024 count=3700K
sudo losetup --show -f $IMG_FILE
sudo fdisk /dev/loop0 << EOF
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

sudo partprobe $DRIVE

echo "creating file systems"

sudo mkfs.vfat -F 32 -n "BOOT" ${DRIVE}p1
sudo mke2fs -j -L "rootfs" ${DRIVE}p2

sync
sudo partprobe $DRIVE
sync
}


echo "#---------------------------------------------------------------------------------- "
echo "#-------------make_uboot_spl-img.sh Part Finished --------------------------------- "
echo "#---------------------------------------------------------------------------------- "

export CROSS_COMPILE=$CC

mkdir -p $kerneldir
cd $kerneldir
git clone https://github.com/altera-opensource/linux-socfpga.git rocketboards
cd rocketboards 
git remote add rocketboards https://github.com/altera-opensource/linux-socfpga.git


# git remote show rocketboards
git fetch rocketboards
git checkout -b linux-rt rocketboards/socfpga-3.10-ltsi-rt

# make socfpga_defconfig
make mrproper


make ARCH=arm socfpga_defconfig 2>&1 | tee ../linux-config_rt-log.txt
# zImage:
make -j$NCORES ARCH=arm 2>&1 | tee ../linux-make_rt-log_.txt

# modules:
make -j$NCORES ARCH=arm modules 2>&1 | tee ../linux-modules_rt-log.txt
#sudo make ARCH=arm INSTALL_MOD_PATH=/mnt/rootfs modules_install

#make ARCH=arm -j$NCORES LOADADDR=0x8000 all 2>&1 | tee ../linux-all-log.txt

echo "#---------------------------------------------------------------------------------- "
echo "#-------- socfpga-debian-jessie--kernel-gen.sh Part Finished ---------------------- "
echo "#---------------------------------------------------------------------------------- "

#-----------------------------------------------------------------------------------
# install files
#-----------------------------------------------------------------------------------
create_imagefile

#sudo partprobe $DRIVE
#sync

sudo mount -o uid=1000,gid=1000 ${DRIVE}p1 /mnt/boot

echo "copying boot sector files"
sudo cp arch/arm/boot/zImage /mnt/boot
sudo cp arch/arm/boot/dts/socfpga_cyclone5.dts /mnt/boot/socfpga.dts
sudo cp arch/arm/boot/dts/socfpga_cyclone5.dtb /mnt/boot/socfpga.dtb
sudo umount /mnt/boot

sudo mkdir -p /mnt/rootfs
sudo mount ${DRIVE}p2 /mnt/rootfs
cd $targetdir
sudo tar cf - . | (sudo tar xvf - -C /mnt/rootfs)
cd $kerneldir/rocketboards
#export ARCH=arm
#export PATH=$CC_DIR/bin/:$PATH
#export CROSS_COMPILE=$CC

sudo make ARCH=arm INSTALL_MOD_PATH=/mnt/rootfs modules_install

#sudo make -j$NCORES LOADADDR=0x8000 modules_install INSTALL_MOD_PATH=/mnt/rootfs

sudo umount /mnt/rootfs
sudo losetup -D
sync

###############################################################
# Install boot loader and copy boot sector files
#sudo dd if=u-boot-with-spl-dtb.sfp  of=${DRIVE}p3
#sync
#sudo mkdir -p /mnt/boot
#sudo mount -o uid=1000,gid=1000 ${DRIVE}p1 /mnt/boot
#echo "copying boot sector files"
#sudo cp arch/arm/boot/zImage /mnt/boot
#sudo cp arch/arm/dts/socfpga_cyclone5_de0_nano_soc.dts /mnt/boot/socfpga.dts
#sudo cp arch/arm/dts/socfpga_cyclone5_de0_nano_soc.dtb /mnt/boot/socfpga.dtb
#sudo umount /mnt/boot
###############################################################
echo "installing u-boot-with-spl"
sudo dd bs=512 if=$UBOOT_SPLFILE of=$IMG_FILE  seek=2048 conv=notrunc
sync

echo "#---------------------------------------------------------------------------------- "
echo "#-------             Image building process complete                       -------- "
echo "#---------------------------------------------------------------------------------- "


