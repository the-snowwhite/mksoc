set ethernet mac u-boot 2015.10 to enable network from u-boot    


    U-Boot 2015.10 (Dec 15 2015 - 13:43:32 +0100)

    CPU:   Altera SoCFPGA Platform
    FPGA:  Altera Cyclone V, SE/A4 or SX/C4, version 0x0
    BOOT:  SD/MMC Internal Transceiver (3.0V)
       Watchdog enabled
    I2C:   ready
    DRAM:  1 GiB
    MMC:   SOCFPGA DWMMC: 0
    *** Warning - bad CRC, using default environment

    In:    serial
    Out:   serial
    Err:   serial
    Model: Terasic DE0-Nano(Atlas)
    Net:   
    Error: ethernet@ff702000 address not set.
    No ethernet found.
    Hit any key to stop autoboot:  0 
    =>     

=> setenv ethaddr 12:07:a5:21:48:00
=> saveenv
=> reset


    resetting ...

    U-Boot SPL 2015.10 (Dec 15 2015 - 13:43:32)
    drivers/ddr/altera/sequencer.c: Preparing to start memory calibration
    drivers/ddr/altera/sequencer.c: CALIBRATION PASSED
    drivers/ddr/altera/sequencer.c: Calibration complete


    U-Boot 2015.10 (Dec 15 2015 - 13:43:32 +0100)

    CPU:   Altera SoCFPGA Platform
    FPGA:  Altera Cyclone V, SE/A4 or SX/C4, version 0x0
    BOOT:  SD/MMC Internal Transceiver (3.0V)
       Watchdog enabled
    I2C:   ready
    DRAM:  1 GiB
    MMC:   SOCFPGA DWMMC: 0
    In:    serial
    Out:   serial
    Err:   serial
    Model: Terasic DE0-Nano(Atlas)
    Net:   eth0: ethernet@ff702000
    Hit any key to stop autoboot:  0 
    => 

