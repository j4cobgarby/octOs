NASM_TARGETS=src/main.asm

all:
	mkdir -p build
	nasm -felf32 src/main.asm -o build/main.o
	i686-elf-gcc -T linker.ld -o iso/boot/octos.bin -ffreestanding -O2 -nostdlib build/main.o -lgcc
	grub-mkrescue -o octos.iso iso
