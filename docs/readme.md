


![Hps/cpu to fpga pathways through bridges](https://github.com/the-snowwhite/mksoc/blob/iotest/docs/pics/Hps-fpga-bridges.png)

Going straight for an axi Interface would mean redesigning the top hostmot2.vhd
module splitting the address bus into 2(in / out).

So initially I have settled on using a basic Avalon interface as this is the most straight forward way to implement an I/O path with 1 16-bit adressbus and 2 unidirectional 32-bit data buses.

![Hostmot2 top instance](https://github.com/the-snowwhite/mksoc/blob/iotest/docs/pics/Hostmot2-vhd_inst.png) 
(I will have to to try giving this image a re-try as it seems a bit flawed)

I would be an idea to build this hm2 top controller module into the qsys design as an ip core instead of having it hanging outside through a bus type interface port?
(having all this as 1 single separate basic design partition).

The link between the hm2 avalon interface ip (in qsys), the devicetree (dts-->dtb) entry, and link to getting memory mapped access in (linux) software is sketched out here:
[Machinekit driver sketch](https://github.com/the-snowwhite/machinekit/blob/iotest/src/hal/drivers/mesa-hostmot2/drv/hm2_cvsoc.c#L23) 

[Avalon Interface IP config file](https://github.com/the-snowwhite/mksoc/blob/iotest/QuartusProjects/ip/hm2reg_io/hm2reg_io_hw.tcl#L76)

[this script creates the following relevant headerfile](https://github.com/the-snowwhite/mksoc/blob/iotest/QuartusProjects/DE0_NANO_SOC_GHRD/generate_hps_qsys_header.sh#L1)

[Headerfile containing all hw address info in customized hps soc ](https://github.com/the-snowwhite/mksoc/blob/iotest/QuartusProjects/DE0_NANO_SOC_GHRD/hps_0.h#L12)

On the near future wish list is to implement partial re-configuration partions with the lower level hm2 modules, giving a more modular (hal like) approach to custom configuring.

Is done with a (mostly) solid unchanging interface structure giving "boxes / slots", block elements can be swapped in / out of....(think 4-8 partions at most) 