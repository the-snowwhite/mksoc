Right now an initial mesa 5i25 test design has been added in the mksoc-iotest branch.


----------


Requirements:

Quartus prime 15.1.1.  (on Debian.)


Git clone and checkout branch:


    git clone https://github.com/the-snowwhite/mksoc.git mkhm2soc

    cd mkhm2soc

    git checkout iotest

    
    
copy lpm_components.vhd into <reporoot>/hm2/pins/
ie:

    cp ~/altera/15.1/quartus/libraries/vhdl/lpm/lpm_pack.vhd ~/Development/the-snowwhite-git/mkhm2soc/hm2/pins/~~
    

Open Quartus shell in new konsole and run make all in project folder:

    
    mib@debian9-ws:~$ ~/altera/15.1/embedded/embedded_command_shell.sh
    WARNING: DS-5 install not detected. SoC EDS may not function correctly without a DS-5 install.
    ------------------------------------------------
    Altera Embedded Command Shell

    Version 15.1 [Build 185]
    ------------------------------------------------
    mib@debian9-ws:~$ cd /home/mib/Development/the-snowwhite-git/mkhm2soc/QuartusProjects/DE0_NANO_SOC_GHRD
    mib@debian9-ws:~/Development/the-snowwhite-git/mkhm2soc/QuartusProjects/DE0_NANO_SOC_GHRD$ make all
    
Build time around 10 or so .... ends with:

    Info: Quartus Prime Convert_programming_file was successful. 0 errors, 0 warnings
    Info: Peak virtual memory: 398 megabytes
    Info: Processing ended: Tue Jan 12 21:45:46 2016
    Info: Elapsed time: 00:00:01
    Info: Total CPU time (on all processors): 00:00:01
    tar -czf sd_fat.tar.gz u-boot.scr boot.script output_files/soc_system.rbf soc_system.dtb

Output is collected in this archive.

To install on sd boot folder Move to script folder:

    cd ../../scripts

Insert your Mksoc os sd card and dry run script: 

    ./inst-rbf-dtb.sh

Look for the dev name of your sd-card (!! double check !!)

then install:

    sudo ./inst-rbf-dtb.sh /dev/sdx1
    
and boot .....

