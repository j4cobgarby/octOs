CC = i686-elf-gcc
ASM = nasm
CFLAGS = -Wall -Wextra -ffreestanding
ASMFLAGS = -felf32

OBJS := \
kio.c.o \
pmm.c.o \
asm_procs.c.o \
main.asm.o \

all: octos.iso

test:
	echo $(BUILDDIR)

clear:
	rm -rf build

octos.iso: $(OBJECTS)
	$(CC) -T src/arch/i386/linker.ld -o 

#%.c.o: %.c
	#$(CC) -c -o $@ $< $(CFLAGS) -T src/arch/i386/linker.ld -o 

%.asm.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@
