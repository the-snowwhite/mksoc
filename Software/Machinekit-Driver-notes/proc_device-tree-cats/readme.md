As the mk_socfpga_io_hm2 interface (ip) is registered in the (included) device tree, following info is available from boot:


    machinekit@mksoc:~$ ls '/proc/device-tree/sopc@0/bridge@0xc0000000/hm2-socfpga@0x100040000'
    address_width  clocks  compatible  data_width  name  reg

this folder contains binary cats of the content of each entity.
    
    
The standard socfpga GHRD / linux kernel setup has generic uio module enabled as default, so when the custom(included) uio driver module is loaded:

    sudo insmod hm2reg_uio-module.ko
    

    machinekit@mksoc:~$ lsmod
    Module                  Size  Used by
    hm2reg_uio_module       3041  0
    gpio_altera             4763  0


    
/dev/uio0 appeares and it is then mmappable from userspace.    
