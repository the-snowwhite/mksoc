# mksoc
Repository for Soc Machinekit ports

(Right now this repo only server as an container for future development and initial historic purposes)

The mksoc name contains the broad inclusive scope of running machinekit on any soc based platform,

with or without fpga.


Initially the link to machinekit is provided by the open sourced hostmot2 hdl cores from Mesa, which is tested and proven to be reliable and also provides compatable physical I/O interfaces that require litte work to connect. 


This whole structure  also targets soc and fpga on separate chips If such opensource projects are to emerge.


This is where the development of the Cyclone V mksocfpga project started, so it seemed natuaral to create

a more limited scope named repo called mksocfpga to move this platform to:


The mksocfpga name scope is limited down to: running Machinekit on Soc's where cpu and fpga are in same chip.

This only includes the Altera and Xilinx socfpga platforms currently.

Starting from the bottom of low cost availability effort has so far been concentrated on Alteras Cyclone V soc family

Manifested thru the lowest cost DE0 Nano Soc board from Terasic with the alias Atlas Soc in the Rocketboards.org eco system.

Terasic maintain a different support and tutorial ecosystem via their web: http://www.terasic.com.tw/

It is hoped to gain traction for adding a Zync based platform simmalar to the DE0 Nano Soc, by a person with such a board and experience with the Xilinx tool's.


! Current and updated souces are at:
mksocfpga repo instead-->
https://github.com/the-snowwhite/mksocfpga


-----------------------

Historic info:

Quartus makefile in mksoc/QuartusProjects/DE0_NANO_SOC_GHRD working

    cd QuartusProjects/DE0_NANO_SOC_GHRD
    make all
    
Scripts:    
    
[Generate sd card image](./scripts/Divided_scripts/readme.md)

~~[Install Quartus generated files to sd card](./Notes/install_Makefile-generated-files-to-sdcard.txt)~~

[Install Quartus generated files to sd card](./scripts/inst-rbf-dtb.sh)

~~[Install and Compile Machinekit](./scripts/mksoc-jessie-mk_rip_build-instal-v2.sh)~~

Just install the Machinekit postix Jessie armhf packages, (minus the kernel), and you are good to go...(once the mk_socfpga uio driver is finished and merged)
  
