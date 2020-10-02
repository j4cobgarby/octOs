NASM_TARGETS=src/main.asm
CFLAGS=-Wall -ffreestanding -T linker.ld

all:
	mkdir -p build

	# building assembly
	nasm -felf32 src/kernel/main.asm -o build/main.o

	# building C
	i686-elf-gcc $(CFLAGS) -c src/kernel/kio.c -o build/kio.o -I src/include
	i686-elf-gcc $(CFLAGS) -c src/kernel/pmm.c -o build/pmm.o -I src/include
	i686-elf-gcc $(CFLAGS) -c src/kernel/asm_procs.c -o build/asm_procs.o -I src/include

	# linking
	i686-elf-gcc -T linker.ld -o iso/boot/octos.bin -ffreestanding -nostdlib build/main.o build/kio.o build/pmm.o build/asm_procs.o -lgcc
	grub-mkrescue -o octos.iso iso
	yes | bximage -mode=resize -hd=483M -imgmode=flat -q octos.iso octos.img
