NASM_TARGETS=src/main.asm

all:
	mkdir -p build
	nasm -felf32 src/kernel/main.asm -o build/main.o
	nasm -felf32 src/userspace/init.asm -o build/userinit.o
	i686-elf-gcc -T linker.ld -o iso/boot/octos.bin -ffreestanding -O2 -nostdlib build/main.o build/userinit.o -lgcc
	grub-mkrescue -o octos.iso iso
	yes | bximage -mode=resize -hd=483M -imgmode=flat -q octos.iso octos.img
