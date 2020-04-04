NASM_TARGETS=src/main.asm

all:
	mkdir -p build

	# building assembly
	nasm -felf32 src/kernel/main.asm -o build/main.o
	nasm -felf32 src/userspace/init.asm -o build/userinit.o

	# building c
	i686-elf-gcc -c src/userspace/entry.c -o build/c_entry.o

	# linking
	i686-elf-gcc -T linker.ld -o iso/boot/octos.bin -ffreestanding -nostdlib build/main.o build/userinit.o build/c_entry.o -lgcc
	grub-mkrescue -o octos.iso iso
	yes | bximage -mode=resize -hd=483M -imgmode=flat -q octos.iso octos.img
