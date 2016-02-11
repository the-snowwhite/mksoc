# mksoc
Repository for Cyclone V Soc Machinekit port

! for current quick setup testing goto:
mksocfpga repo instead-->
https://github.com/the-snowwhite/mksocfpga


Quartus makefile in mksoc/QuartusProjects/DE0_NANO_SOC_GHRD working

    cd QuartusProjects/DE0_NANO_SOC_GHRD
    make all
    
Scripts:    
    
[Generate sd card image](./scripts/Divided_scripts/readme.md)

~~[Install Quartus generated files to sd card](./Notes/install_Makefile-generated-files-to-sdcard.txt)~~

[Install Quartus generated files to sd card](./scripts/inst-rbf-dtb.sh)

~~[Install and Compile Machinekit](./scripts/mksoc-jessie-mk_rip_build-instal-v2.sh)~~

Just install the Machinekit postix Jessie armhf packages, (minus the kernel), and you are good to go...(once the mk_socfpga uio driver is finished and merged)
  
