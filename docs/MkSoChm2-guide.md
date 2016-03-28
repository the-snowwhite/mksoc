Right now the quartus design projectfiles behind the initial mesa 5i25 test design in the first socfpga machinekit sd-images is residing in the mksocpga repo.

It has an avalon --> uio driver interface, ~~that hopefully shortly will be replaced with an axi interfacce.~~

Requirements to re-build the required quartus binaries:

From Altera --> Download --> (update version)

Quartus prime 15.1.2  (on Debian.)

Soc Eds

Git clone:


    git clone https://github.com/the-snowwhite/mksocfpga.git mkhm2soc

    cd mkhm2soc


Open Quartus shell in new konsole and run make all in project folder:

    
    mib@debian9-ws:~$ ~/altera/15.1/embedded/embedded_command_shell.sh
    WARNING: DS-5 install not detected. SoC EDS may not function correctly without a DS-5 install.
    ------------------------------------------------
    Altera Embedded Command Shell

    Version 15.1 [Build 193]
    ------------------------------------------------
    mib@debian9-ws:~$ cd /home/mib/Development/the-snowwhite-git/mkhm2soc/QuartusProjects/DE0_NANO_SOC_GHRD
    mib@debian9-ws:~/Development/the-snowwhite-git/mkhm2soc/QuartusProjects/DE0_NANO_SOC_GHRD$ make dts dtb sof
    
Build time around 10 min or so .... ends with:

    Info: Quartus Prime Convert_programming_file was successful. 0 errors, 0 warnings
    Info: Peak virtual memory: 398 megabytes
    Info: Processing ended: Tue Jan 12 21:45:46 2016
    Info: Elapsed time: 00:00:01
    Info: Total CPU time (on all processors): 00:00:01
    tar -czf sd_fat.tar.gz u-boot.scr boot.script output_files/soc_system.rbf soc_system.dtb

Output is collected in this archive.

To install into sd boot folder:

look at this script:

https://github.com/the-snowwhite/soc-image-buildscripts/blob/master/inst-rbf-dtb.sh
