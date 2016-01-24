As the mk_socfpga_io_hm2 interface (ip) is registered in the (included) device tree, 
following hm2 related interface info is included in the socfpga.dtb file loaded by the bootloader.


    machinekit@mksoc:~$ ls '/proc/device-tree/sopc@0/bridge@0xc0000000/hm2-socfpga@0x100040000'
    address_width  clocks  compatible  data_width  name  reg

this folder contains raw binary cats of the content of each entity.

Here in ascii / text converted form:

address_width	00 00 00 0E
clocks			00 00 00 02
compatible		6D 61 63 68 6B 74 2C 68 6D 32 72 65 67 2D 69 6F 2D 31 2E 30 00
				machkt,hm2reg-io-1.0.
data_width		00 00 00 20
name			68 6D 32 2D 73 6F 63 66 70 67 61 00
				hm2-socfpga.
reg				00 00 00 01 00 04 00 00 00 01 00 00
    
The standard socfpga GHRD / linux kernel setup has generic uio module enabled as default, so when the custom(included) uio driver module is loaded:

    sudo insmod hm2reg_uio-module.ko
    
    machinekit@mksoc:~$ lsmod
    Module                  Size  Used by
    hm2reg_uio_module       3041  0
    gpio_altera             4763  0

/dev/uio0 appeares and it is then mmappable from userspace.    
