    Notes about manually generating the SD-card image + (boot loaders, kernel and jessie rootfs)

    And first boot preparations:



socfpga-debian-jessie--sd-image-gen.sh

[Invokes other selected scripts in same folder that generates a armhf Debian Jessie sd-card-image(.img)
for the Terasic De0 Nano / Altera Atlas Soc-Fpga dev board] (https://github.com/the-snowwhite/mksoc/blob/master/scripts/Divided_scripts/socfpga-debian-jessie--sd-image-gen.sh)

    Overview of manual install process, look around for (updated) details and scripts ...

Files are created in the current terminal path the script is called from.

then:

On Pc:

1. copy the sd-image to sd-card

2. Install the socfpga.dtb and socfpga.rbf files into the (fat..)boot partition.


on NaNo / Atlas board

1. attach usb serial console in minicom or similar (115 8n1 kbaud), and be ready to tap keyboard in terminal.

2. Boot and stop at u-boot prompt (tap key < 3-5 sec)

3. Input the environment variables needed to load the .rbf config file in u-boot pre kernel boot.

4. Then you can proceed to install the Machinekit armhf Jessie packages. and begin testing the current NANO /Atlas Mesa 5i25    HW emulator/clone config.

---

    Right now.
    Assistance for hooking up the soc hm2 uio mmaped driver interface
    to the hostmot2 hal drivers in Machine would be greatly appreciated.


----

~~[Script for Installing / compiling full Machinekit armhf Debian Jessie rip build] (https://github.com/the-snowwhite/mksoc/blob/master/scripts/mksoc-jessie-mk_rip_build-instal-v2.sh)~~

~~[Latest version](https://github.com/the-snowwhite/mksoc/blob/master/scripts/generic-jessie-mk_rip_fresh-build-instal-beta.sh)~~

tested and works in x84_64 chroot
