CC = i686-elf-gcc
ASM = nasm
CFLAGS = -Wall -Wextra -ffreestanding -I include
ASMFLAGS = -felf32

OBJS = kio.c.o \
pmm.c.o \
asm_procs.c.o \
main.asm.o \

all: octos.iso

test:
	echo $(BUILDDIR)

clean:
	rm -rf ../../build

octos.iso: $(OBJS)
	echo $(OBJS)
	cp -r ../../static/ ../../build
	$(CC) -T src/arch/i386/linker.ld -o ../../build/iso/octos.bin -ffreestanding -nostdlib $^ -lgcc
	grub-mkrescue -o ../../build/octos.iso ../../build/iso

%.c.o: src/arch/i386/%.c
	mkdir -p ../../build
	$(CC) -c -o ../../build/$@ $< $(CFLAGS) -T src/arch/i386/linker.ld

%.asm.o: src/arch/i386/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@
