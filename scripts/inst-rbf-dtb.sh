#!/bin/sh

projects=$(ls ../QuartusProjects)

mkdir -p output_files
for folder in $projects
do
#        tar -zxvf ../QuartusProjects/$folder/sd_fat.tar.gz soc_system.dtb
#        mv soc_system.dtb $folder.dtb
#        tar -zxvf ../QuartusProjects/$folder/sd_fat.tar.gz output_files/*.rbf
#        mv output_files/*.rbf output_files/$folder.rbf
#        tar -zxvf ../QuartusProjects/$folder/sd_fat.tar.gz soc_system.dtb
        cp -v ../QuartusProjects/$folder/*.dtb output_files/socfpga.dtb 
#        tar -zxvf ../QuartusProjects/$folder/sd_fat.tar.gz output_files/*.rbf
        cp -v ../QuartusProjects/$folder/output_files/*.rbf output_files/socfpga.rbf
done

if [ "$1" != "" ]; then
        lsblk
        echo ""
        echo "files will be installed on $1"
        echo ""
        sudo mkdir -p /mnt/boot
        sudo mount -o uid=1000,gid=1000 $1 /mnt/boot
#        sudo cp *.dtb /mnt/boot
        sudo cp -fv output_files/socfpga* /mnt/boot
#        sudo cp u-boot.scr /mnt/boot
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

