Here are working work in progress, scripts for cross building a fully bootable Debian sd-card image. ( Jessie onwards).

full / partial build can be commented in / out at bottom of script:

mksoc/scripts/Divided_scripts/socfpga-debian-jessie--sd-image-gen.sh

[Invokes selected scripts in same folder that generates a working armhf Debian Jessie sd-card-image().img
for the Terasic De0 Nano / Altera Atlas Soc-Fpga dev board] (https://github.com/the-snowwhite/mksoc/blob/master/scripts/Divided_scripts/socfpga-debian-jessie--sd-image-gen.sh)

Importaint remember to make sure u-boot is loading the (.rbf) config file before booting the kernel:

    #-- missing config fpga on u-boot boot fix ----------------------------#
    setenv fpgaimage socfpga.rbf
    setenv fpgadata 0x2000000

    setenv mmcload 'mmc rescan;load mmc 0:1 ${fpgadata} ${fpgaimage};load mmc 0:1 ${loadaddr} ${bootimage};load mmc 0:1 ${fdt_addr} ${fdtimage}'

    setenv fpgaconfig 'load mmc 0:1 ${fpgadata} ${fpgaimage};fpga load 0 ${fpgadata} ${filesize}'

    setenv bootcmd 'run fpgaconfig; run mmcload; run mmcboot'

    saveenv

    #-- missing lan @u-boot fix -- permanent until preloader reinstall ----#

    setenv ethaddr xx:xx:xx:xx:xx:xx

    saveenv


    #----->
    reset