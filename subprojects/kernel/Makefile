CC = i686-elf-gcc
ASM = nasm
CFLAGS = -Wall -Wextra -ffreestanding -I include -DKERNEL_DEBUG -g \
-mno-red-zone -mgeneral-regs-only
ASMFLAGS = -felf32 -i src/arch/i386/

OBJS = kio.c.o \
pmm.c.o \
paging.c.o \
asm_procs.c.o \
main.asm.o \
exception_handlers.c.o \
idt.c.o \
ata_pio.c.o

all: octos.img

clean:
	rm -rf ../../build

octos.img: octos.iso
	rm -f ../../build/octos.img
	cd ../../build && \
	bximage -mode=resize -hd=483M -imgmode=flat -q octos.iso octos.img

octos.iso: $(OBJS)
	cd ../../build && \
	cp -r ../static/iso . && \
	$(CC) -T ../subprojects/kernel/src/arch/i386/linker.ld -o iso/boot/octos.bin -ffreestanding -nostdlib $^ -lgcc && \
	grub-mkrescue -o octos.iso iso

%.c.o: src/arch/i386/%.c
	mkdir -p ../../build
	$(CC) -c -o ../../build/$@ $< $(CFLAGS) -T src/arch/i386/linker.ld

%.asm.o: src/arch/i386/%.asm
	$(ASM) $(ASMFLAGS) $< -o ../../build/$@
