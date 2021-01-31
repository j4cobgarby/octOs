#!/bin/bash

# ran from the build directory

dd if=/dev/zero of=disk.img bs=512 count=131072
sfdisk disk.img < ../disklayout
LOOP0=`sudo losetup -f`
sudo losetup $LOOP0 disk.img
LOOP1=`sudo losetup -f`
sudo losetup $LOOP1 -o 1048576 disk.img
sudo mkfs.fat -F16 -f2 $LOOP1
sudo mount $LOOP1 /mnt
sudo grub-install --target=i386-pc --root-directory=/mnt --no-floppy --modules="normal part_msdos ext2 fat multiboot" $LOOP0
sudo cp ../static/iso/boot/grub/grub.cfg /mnt/boot/grub/grub.cfg
sudo cp octos.bin /mnt/boot/OCTOS.BIN
sudo umount /mnt
sudo losetup -D