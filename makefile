NASM_TARGETS=src/main.asm

all:
	mkdir -p build

	# building assembly
	nasm -felf32 src/kernel/main.asm -o build/main.o

	# linking
	i686-elf-gcc -T linker.ld -o iso/boot/octos.bin -ffreestanding -nostdlib build/main.o -lgcc
	grub-mkrescue -o octos.iso iso
	yes | bximage -mode=resize -hd=483M -imgmode=flat -q octos.iso octos.img
