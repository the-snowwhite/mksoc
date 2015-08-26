#!/bin/sh

projects=$(ls ../QuartusProjects)

for folder in $projects
do
        tar -zxvf ../QuartusProjects/$folder/sd_fat.tar.gz soc_system.dtb
        mv soc_system.dtb $folder.dtb
        tar -zxvf ../QuartusProjects/$folder/sd_fat.tar.gz output_files/soc_system.rbf
        mv output_files/soc_system.rbf output_files/$folder.rbf
done

echo "auto lo eth0" > boot.script
echo "setenv fpga_do_config 0;" >> boot.script
for folder in $projects
do
        echo 'if test ${fpga_do_config} = 0; then' >> boot.script
        echo "fatload mmc 0:1 \$fpgadata output_files/$folder.rbf;" >> boot.script
        echo "if fpga load 0 \$fpgadata \$filesize; then" >> boot.script
        echo "setenv fdtimage $folder.dtb; setenv fpga_do_config 1;" >> boot.script
        echo "else" >> boot.script
        echo "setenv fpga_do_config 0;" >> boot.script
        echo "fi" >> boot.script
        echo "fi" >> boot.script
done
echo "if test ${fpga_do_config} = 1; then" >> boot.script
echo "run bridge_enable_handoff;" >> boot.script
echo "run mmcload;" >> boot.script
echo "run mmcboot;" >> boot.script
echo "else" >> boot.script
echo "echo Not continuing as ..!!!" >> boot.script
echo "echo no vaild config files for board found ..!!!" >> boot.script
echo "fi" >> boot.script

mkimage  -A arm -O linux -T script -C none -a 0 -e 0 -n "MkSoc bootscript" -d boot.script u-boot.scr

if [ "$1" != "" ]; then
        lsblk
        echo ""
        echo "files will be installed on $1"
        echo ""
        sudo mkdir -p /mnt/boot
        sudo mount -o uid=1000,gid=1000 $1 /mnt/boot
        sudo cp *.dtb /mnt/boot
        sudo cp -rf output_files /mnt/boot
        sudo cp u-boot.scr /mnt/boot
        sync
        sudo umount /mnt/boot
        sync
        echo "operation finished"
else
        lsblk
        echo ""
        echo "! -- not installing files"
        echo "no installation drive given"
        echo "ie: /dev/sdxx"
        echo ""
fi
# cat > boot.script << "EOF"
# fatload mmc 0:1 $fpgadata output_files/soc_system.rbf;
# fpga load 0 $fpgadata $filesize;
# setenv fdtimage soc_system.dtb;
# run bridge_enable_handoff;
# run mmcload;
# run mmcboot;EOF

#tar -zxvf /home/mib/the-snowwhite_git/mksoc/QuartusProjects/sockit_ghrd/sd_fat.tar.gz soc_system.rbf

#fatload mmc 0:1 $fpgadata output_files/soc_system.rbf;


#setenv check 'if fpga load 0 $fpgadata $filesize; then echo OK; else echo Wrong board; fi'

