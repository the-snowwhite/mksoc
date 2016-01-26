DEspite the everything in one folder method the multi configuration Mesa source code
is delivered in. I have grown to admire it's construction and see greet potential
for expansion into the socfpga wildness.

The current method of implemention is ruled by 3 config files (called roms):

note:all links are to pdf prints

1. the this is everything thats possible / allowed file used for probing the correct
hardware board ... 
[IDROMConst.vhd](https://github.com/the-snowwhite/mksoc/blob/master/Software/Machinekit-Driver-notes/Mesa-config_rom-printouts/IDROMConst.pdf) 

2. the (static) parameters passed at compile time
which leads to a single (static)fpga config file, used to program the fpga-logic(socfpga.rbf)
this is the one used for the current mesa 5i25 clone in mksoc repo master branch:
[i25_x9card.vhd](https://github.com/the-snowwhite/mksoc/blob/master/Software/Machinekit-Driver-notes/Mesa-config_rom-printouts/i25_x9card.pdf)

3. this file controls the layout of the ("ram" image)(memory map) that Machinekit sees and interacts with controlling and reading info from the mesa board.

It describes which functions/modules (pwm, steppers etc) are available, how many(compiled in), and which physical pins they are hardwired into.
[PIN_DRINGx2_34.vhd](https://github.com/the-snowwhite/mksoc/blob/master/Software/Machinekit-Driver-notes/Mesa-config_rom-printouts/PIN_DRINGx2_34.pdf)

-----

The hostmot2 core also contains a pin multiplexer that enables total rewiring of the I/O pins from hal.

It took me 3 days (and nights) to port the mesa source code into the working mksoc template (once I saw how to do it)
it took me a weeks time more to detect the problems I was having with the system freezing every time i tried to read the memory location was due to overlooking/forgetting to reenable the autoload of the .rbf fpga config file at boot.
Due to using the newer u-boot 2016.x based sd-image I had developed meanwhile.....

-----

The paths(in levels) of working towards communicating with machinekit in linux is:

sysfs  --> a gateway to accessing all physical memory locations.(not safe and not recomended outside prototyping)

uio generic_irq.c kernel driver --> can provide a memory port to only the devices memory. (cannot pass ( provide any info about the device).

creating a custom version of uio_generic_irq.c (very easy to implement, can provide info via the device tree dtb file from boot time. 
This device-tree info is also not limited to only uio driver use.)

So this seemed like a good place to implement first base. So far so good.

----
End of part 1.

