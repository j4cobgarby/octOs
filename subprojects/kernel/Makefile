CC = i686-elf-gcc
ASM = nasm
CFLAGS = -Wall -Wextra -ffreestanding -I include -I ../klib/include -DKERNEL_DEBUG\
-mno-red-zone -mgeneral-regs-only -O2
ASMFLAGS = -felf32 -i src/arch/i386/
BUILDDIR=../../build

OBJS = kio.o \
pmm.o \
paging.o \
asm_procs.o \
main.asm.o \
exception_handlers.o \
idt.o \
ata_pio.o \
fat16.o \
drives.o \
virtfs.o \
main.o

all: disk.img

clean:
	rm -rf $(BUILDDIR)

disk.img: $(OBJS)
	# Make klib
	cd ../klib && make
	# Link kernel
	cd $(BUILDDIR) && \
	$(CC) -T ../subprojects/kernel/src/arch/i386/linker.ld -o octos.bin -ffreestanding -nostdlib $^ klib/* -lgcc && \
	../makedisk.sh

%.asm.o: src/arch/i386/%.asm
	$(ASM) $(ASMFLAGS) $< -o ../../build/$@

%.o: src/arch/i386/%.c
	mkdir -p $(BUILDDIR)
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS) -T src/arch/i386/linker.ld