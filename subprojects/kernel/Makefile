CC = i686-elf-gcc
ASM = nasm
CFLAGS = -Wall -Wextra -ffreestanding -I include -DKERNEL_DEBUG\
-mno-red-zone -mgeneral-regs-only
ASMFLAGS = -felf32 -i src/arch/i386/
BUILDDIR=../../build

OBJS = kio.c.o \
pmm.c.o \
paging.c.o \
asm_procs.c.o \
main.asm.o \
exception_handlers.c.o \
idt.c.o \
ata_pio.c.o \
FAT12.c.o

all: disk.img

clean:
	rm -rf ../../build

disk.img: $(OBJS)
	cd $(BUILDDIR) && \
	$(CC) -T ../subprojects/kernel/src/arch/i386/linker.ld -o octos.bin -ffreestanding -nostdlib $^ -lgcc && \
	../makedisk.sh

%.c.o: src/arch/i386/%.c
	mkdir -p $(BUILDDIR)
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS) -T src/arch/i386/linker.ld

%.asm.o: src/arch/i386/%.asm
	$(ASM) $(ASMFLAGS) $< -o ../../build/$@
