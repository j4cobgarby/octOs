all:
	mkdir -p build
	nasm -felf32 boot/boot.asm -o build/boot.o
	i686-elf-gcc -c kernel/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i686-elf-gcc -T linker.ld -o iso/boot/octos.bin -ffreestanding -O2 -nostdlib build/boot.o build/kernel.o -lgcc
	grub-mkrescue -o octos.iso iso
